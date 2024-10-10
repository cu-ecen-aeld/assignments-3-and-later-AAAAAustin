#CROSS_COMPILE=aarch64-none-linux-gnu-

all: aesdsocket_link

#aarch64-none-linux-gnu-
aesdsocket_link: aesdsocket_compile
	$(CROSS_COMPILE)gcc aesdsocket.o -o aesdsocket 

aesdsocket_compile: aesdsocket.c
	$(CROSS_COMPILE)gcc -c aesdsocket.c -o aesdsocket.o

clean:
	rm -f aesdsocket
	rm -f *.o

test: 
	gcc -c aesdsocket.c -o aesdsocket.o
