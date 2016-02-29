CFLAGS=-Wall

client: client.c client_function.h client_function.c serv_function.h serv_function.c
	gcc -o $@ client.c client_function.c serv_function.c ${CFLAGS}
