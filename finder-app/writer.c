// Austin Spaulding Assignment 2
// Assume directory is already created
// Use LOG_USER
// LOG_DEBUG level
// LOG_ERR

#include <stdio.h>
#include <syslog.h>
#include <string.h>

int main (int argc, char *argv[]) {

	// sysilog(LOG_USER, "OK")

	openlog(NULL, LOG_PID, LOG_USER);

	if ( argc < 3 ) {		
		syslog(LOG_ERR, "ERROR!!! INCORRECT NUMBER OF PARAMETERS");
		closelog();
		return 1;
	}

	else {
		const char *writefile = argv[1];
		const char *writestr = argv[2];

		FILE *file = fopen(writefile, "w");
		fwrite( writestr, sizeof(char), strlen(writestr), file );

		syslog(LOG_DEBUG, "Writing %s to %s", writestr, writefile);
	}

}
