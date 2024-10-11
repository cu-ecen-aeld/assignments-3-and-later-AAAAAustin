#Core makefile
all: writer_link

#aarch64-none-linux-gnu-gcc
writer_link: writer_compile
	$(CROSS_COMPILE)gcc writer.o -o writer 

writer_compile: writer.c
	$(CROSS_COMPILE)gcc -c writer.c -o writer.o

clean:
	rm -f writer
	rm -f *.o

#Debugging
test_writer: writer.c writer.o writer
	gcc -c writer.c -o writer.o
	gcc writer.o -o writer
	./writer ./test.txt "test"

test_writer_err: writer.c writer.o writer
	gcc -c writer.c -o writer.o
	gcc writer.o -o writer
	./writer ./test.txt
