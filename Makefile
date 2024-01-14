CC = gcc
CCFLAGS = -Wall
EXEC = server client

all: $(EXEC)

server : server.c
	$(CC) $(CCFLAGS) -o server server.c

client : client.c
	$(CC) $(CCFLAGS) -o client client.c

clean:
	rm -f *.o
mproper:
	rm -f server client *.o