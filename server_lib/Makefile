CFLAGS=-Wall
EXEC=serveur

all: ${EXEC}

serveur: server.c utilities.c server_function.h server_function.c db.c db.h utilities.h
	gcc -o $@ server.c db.c server_function.c utilities.c ${CFLAGS} `mysql_config --cflags --libs`
