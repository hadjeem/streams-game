CC=gcc

CFLAGS=-D_DEFAULT_SOURCE -D_XOPEN_SOURCE -D_BSD_SOURCE -std=c11 -pedantic -Wvla -Wall -Werror -g

ALL=server client

all: $(ALL)

server: server.o utils_v1.o game.o network.o ipc.o
	$(CC) $(CFLAGS) -g -o server server.o utils_v1.o game.o network.o ipc.o

client: client.o utils_v1.o game.o network.o
	$(CC) $(CFLAGS) -o client client.o utils_v1.o game.o network.o

server.o: server.c utils_v1.h config.h
	$(CC) $(CFLAGS) -c server.c

client.o: client.c utils_v1.h config.h
	$(CC) $(CFLAGS) -c client.c

game.o: game.c utils_v1.h
	$(CC) $(CFLAGS) -c game.c

network.o: network.c utils_v1.h
	$(CC) $(CFLAGS) -c network.c

ipc.o: ipc.h utils_v1.h
	$(CC) $(CFLAGS) -c ipc.c

utils_v1.o: utils_v1.c utils_v1.h
	$(CC) $(CFLAGS) -c utils_v1.c

clean:
	rm -f $(ALL)
	rm -f *.o

dev:
	ipcrm -a