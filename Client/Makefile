OS := $(shell uname)
ifeq ($(OS),Darwin)
	CFLAGS=-Wall -g -lpthread -framework OpenAL -lsndfile -DPROJ=1
else
	CFLAGS=-Wall -g -lpthread -lopenal -lsndfile -DPROJ=1
endif

all: client clean

client: message.h message.c protocole.h protocole.c client.c client_function.h client_function.c serv_function.h serv_function.c transfert.c annuaire.c annuaire.h online.c online.h
	gcc -c message.c
	gcc -c protocole.c
	gcc -c client_function.c
	gcc -c serv_function.c
	gcc -c annuaire.c
	gcc -c client.c
	gcc -c transfert.c
	gcc -c online.c
	g++ -c OpenAL_capture.cpp
	g++ -c OpenAL_lecture.cpp
	g++ -Wall -g -o $@ message.o protocole.o client.o client_function.o serv_function.o transfert.o annuaire.o online.o OpenAL_capture.o OpenAL_lecture.o ${CFLAGS}

clean:
	rm -rf *.o

mrproper: clean
	rm -rf client


ecole: client_ecole clean
	

client_ecole: message.h message.c protocole.h protocole.c client.c client_function.h client_function.c serv_function.h serv_function.c transfert.c annuaire.c annuaire.h online.c online.h
	gcc -c message.c
	gcc -c protocole.c
	gcc -c client_function.c
	gcc -c serv_function.c
	gcc -c annuaire.c
	gcc -c client.c
	gcc -c transfert.c
	gcc -c online.c
	gcc -Wall -g -o $@ message.o protocole.o client.o client_function.o serv_function.o transfert.o annuaire.o online.o -Wall -g -lpthread

