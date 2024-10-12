#CROSS_COMPILE=aarch64-none-linux-gnu-
$(CC)=$(CROSS_COMPULE)gcc

all: aesdsocket_link

#aarch64-none-linux-gnu-
aesdsocket_link: aesdsocket_compile
	$(CC) aesdsocket.o -o aesdsocket 

aesdsocket_compile: aesdsocket.c
	$(CC) -c aesdsocket.c -o aesdsocket.o

clean:
	rm -f aesdsocket
	rm -f *.o

test: 
	gcc -c aesdsocket.c -o aesdsocket.o
