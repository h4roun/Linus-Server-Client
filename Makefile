CC = gcc
CFLAGS = -Wall -g

all: serveur client

serveur: serveur.c serv_cli_fifo.h Handlers_Serv.h
	$(CC) $(CFLAGS) -o serveur serveur.c

client: client.c serv_cli_fifo.h Handlers_Cli.h
	$(CC) $(CFLAGS) -o client client.c

clean:
	rm -f serveur client
	rm -f /tmp/fifo1 /tmp/fifo2

.PHONY: all clean