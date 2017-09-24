CC=/usr/bin/cc

# Globals 

all: generate sender receiver

clean:
	rm -f *.o sender receiver generate

# Object Files 

common.o: common.c common.h
	$(CC) -Wall -c common.c 

generate.o: generate.c
	$(CC) -Wall -c generate.c

sender.o: sender.c
	$(CC) -Wall -c sender.c

receiver.o: receiver.c
	$(CC) -Wall -c receiver.c

# Executables

generate: generate.o common.o
	$(CC) -Wall -o generate common.o generate.o

sender: sender.o common.o
	$(CC) -Wall -o sender common.o sender.o

receiver: common.o receiver.o
	$(CC) -Wall -g -o receiver common.o receiver.o
