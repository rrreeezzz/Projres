CFLAGS=-Wall

all: client serveur

serveur: server.c server.h
	gcc -o serveur server.c ${CFLAGS}

client: client.c client.h
	gcc -o client client.c ${CFLAGS}

