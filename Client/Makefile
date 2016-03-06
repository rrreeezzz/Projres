CFLAGS=-Wall -g

all: client clean mrproper

client: message.h message.c protocole.h protocole.c client.c client_function.h client_function.c serv_function.h serv_function.c
	gcc -c message.c
	gcc -c protocole.c
	gcc -c client_function.c
	gcc -c serv_function.c
	gcc -c client.c
	gcc -o $@ message.o protocole.o client.o client_function.o serv_function.o ${CFLAGS}

clean:
	rm -rf *.o

mrproper: clean
	rm -rf cient
