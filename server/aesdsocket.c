// obtained from beej.us guide
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdbool.h>
#include <syslog.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fs.h>

#define PORT "9000"
#define BACKLOG 10
#define BUFFER_MAX_SIZE 100

bool caught_sigint_f = true;

int packet_handling(int *newfd); 
static void signal_handler ( int signal_number );
void graceful_exit();

// global vars so we can exit gracefully later on
int sockfd;
int newfd;
FILE *file_ptr;

int main(int argc, char *argv[]){

	int status;
	char client_addr[INET6_ADDRSTRLEN];
	int num_bytes;
	int yes = 1;
	struct addrinfo hints;
	struct addrinfo *servinfo_valid;
	struct addrinfo *servinfo;
	// struct sockaddr_storage their_addr;
	socklen_t sin_size;
	struct sigaction new_action;
	bool success = true;

	// signal handling initialization
	memset(&new_action, 0, sizeof(struct sigaction));
	new_action.sa_handler=signal_handler;
	if ( sigaction(SIGINT, &new_action, NULL) == -1 ) {
		syslog(LOG_ERR, "ERROR sigaction SIGINT");
		graceful_exit();
		exit(EXIT_FAILURE);
	}
	if ( sigaction(SIGTERM , &new_action, NULL) == -1 ) {
		syslog(LOG_ERR, "ERROR sigaction SIGTERM");
		graceful_exit();
		exit(EXIT_FAILURE);
	}

	// creating/overwriting tmp file
	if ( (file_ptr = fopen("/var/tmp/aesdsocketdata", "w")) == NULL ) {
		syslog(LOG_ERR, "ERROR opening var/tmp/aesdsocketdata");
		graceful_exit();
		exit(EXIT_FAILURE);
	}
	fclose(file_ptr);

	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_INET6;      // IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

	// setting up sockets start
	if ( (status = getaddrinfo(NULL, PORT, &hints, &servinfo) ) != 0) {
    	syslog(LOG_ERR, "ERROR getaddrinfo: %s\n", gai_strerror(status));
		graceful_exit();
    	exit(EXIT_FAILURE);
	}

	for ( servinfo_valid = servinfo; servinfo_valid != NULL; servinfo_valid = servinfo_valid->ai_next ) {
		
		if ( ( sockfd = socket(servinfo_valid->ai_family, servinfo_valid->ai_socktype, servinfo_valid->ai_protocol) ) == -1 ) {
			syslog(LOG_ERR, "server: socket");	
			graceful_exit();
			continue;
		}
		
		if ( setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1 ) {
			syslog(LOG_ERR, "ERROR setsockopt");
			graceful_exit();
			exit(EXIT_FAILURE);
		}
		
		if ( bind(sockfd, servinfo_valid->ai_addr, servinfo_valid->ai_addrlen) == -1 ) {
			close(sockfd);
			graceful_exit();
			syslog(LOG_ERR, "ERROR bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo);
	// setting up sockets end

	// daemon stuff
	// taken from linux System Programming p.160
	if (argc > 1 && strcmp(argv[1], "-d") == 0) {
		pid_t pid = fork();
		if ( pid < 0 ) {
			syslog(LOG_ERR, "ERROR fork");
			graceful_exit();
			exit(EXIT_FAILURE);
		}
		if ( pid > 0 ) {
			exit(EXIT_SUCCESS);
		}
		if ( setsid() < 0 ) {
			syslog(LOG_ERR, "ERROR setsid");
			graceful_exit();
			exit(EXIT_FAILURE);
		}
		if ( chdir("/") < 0 ) {
			syslog(LOG_ERR, "ERROR chdir");
			graceful_exit();
			exit(EXIT_FAILURE);
		}

		open ("/dev/null", O_RDWR); /* stdin */
		dup (0); /* stdout */
		dup (0); /* stderror */
	}
	// end linux System Programming p.160

	// listening for socket connection
	if ( listen(sockfd, BACKLOG) == -1 ) {
		syslog(LOG_ERR, "ERROR listen");
		graceful_exit();
		exit(EXIT_FAILURE);
	}
	
	if (servinfo_valid == NULL) {
		syslog(LOG_ERR, "ERROR bind");
		graceful_exit();
		exit(EXIT_FAILURE);
	}

	// looping over new packets until sigint
	while(caught_sigint_f) {
		
		struct sockaddr_in their_addr;
		sin_size = sizeof(their_addr);
		
		newfd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (newfd == -1) {
			syslog(LOG_ERR, "ERROR accept");
			graceful_exit();
			continue;
		}

		struct sockaddr_in *addr = (struct sockaddr_in *)&their_addr;
		
		if ( inet_ntop(AF_INET, &addr->sin_addr, client_addr, sizeof(client_addr)) == NULL) {
			syslog(LOG_ERR, "ERROR inet_ntop");
			graceful_exit();
			exit(EXIT_FAILURE);	
		}

		syslog(LOG_DEBUG, "Accepted connection from %s\n", client_addr);

		if ( packet_handling(&newfd) != 0 ) {
			syslog(LOG_ERR, "ERROR packet handling error");
			graceful_exit();
			exit(EXIT_FAILURE);
		}

		syslog(LOG_DEBUG, "Closed connection from %s\n", client_addr);
	}

	graceful_exit();
	exit(EXIT_SUCCESS);

}

int packet_handling(int *newfd) {


	char rx_buffer [BUFFER_MAX_SIZE];

	if ( (file_ptr = fopen("/var/tmp/aesdsocketdata", "a+")) == NULL ) {
		perror("Error opening file");
		return -1;
	}

	int num_bytes;
	while ( (num_bytes = recv(*newfd, rx_buffer, sizeof(rx_buffer), 0)) > 0 )
	{
		// printf("Testing, %d\n", num_bytes);

		rx_buffer[num_bytes] = '\0';
		fputs(rx_buffer, file_ptr);
		fflush(file_ptr);
		
		char tx_buffer [BUFFER_MAX_SIZE];
		if ( strchr(rx_buffer, '\n') ) {

			fseek(file_ptr, 0, SEEK_SET);
			while ((num_bytes = fread(tx_buffer, 1, sizeof(rx_buffer), file_ptr)) > 0)
			{
				tx_buffer[num_bytes] = '\0';
				send(*newfd, tx_buffer, num_bytes, 0);
			}

		}

	}

	return 0;

}	

static void signal_handler ( int signal_number ) {

	graceful_exit();
	caught_sigint_f = false;
	exit(EXIT_SUCCESS);

}

void graceful_exit() {

	if ( file_ptr != NULL ) {
		fclose(file_ptr);
	}

	// Following code from stack exchange https://stackoverflow.com/questions/4142012/how-to-find-the-socket-connection-state-in-c
	// Using getsockopt, checking if the sockets are open so we can close if they are

	int len;
	int error;
	int isopen = getsockopt (sockfd, SOL_SOCKET, SO_ERROR, &error, &len); 

	if ( isopen != 0 ) {
		close(sockfd);
	}

	isopen = getsockopt (newfd, SOL_SOCKET, SO_ERROR, &error, &len); 

	if ( isopen != 0 ) {
		close(newfd);
	}

}
