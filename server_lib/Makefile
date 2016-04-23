CFLAGS=-Wall
EXEC=serveur

all: ${EXEC}

serveur: server.c utilities.c server_function.h server_function.c db.c db.h utilities.h message.c message.h
	gcc -o $@ server.c utilities.c message.c db.c server_function.c ${CFLAGS} `mysql_config --cflags --libs`
