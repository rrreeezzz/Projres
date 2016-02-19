CFLAGS=-Wall
EXEC=serveur client

all: ${EXEC}

serveur: ./server_lib/server.c ./server_lib/server_function.h ./server_lib/server_function.c ./server_lib/db.c ./server_lib/db.h
	gcc -o $@ ./server_lib/server.c ./server_lib/db.c ./server_lib/server_function.c ${CFLAGS} `mysql_config --cflags --libs`

client: ./client_lib/client.c ./client_lib/client.h
	gcc -o $@ ./client_lib/client.c ${CFLAGS}
