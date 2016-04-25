OS := $(shell uname)
ifeq ($(OS),Darwin)
	CFLAGS=-Wall -g -lpthread -framework OpenAL -lsndfile
else
	CFLAGS=-Wall -g -lpthread -lopenal -lsndfile
endif

all: client clean

client: 
	gcc -c message.c -DPROJ
	gcc -c protocole.c -DPROJ
	gcc -c client_function.c
	gcc -c serv_function.c -DPROJ
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
	

client_ecole:
	gcc -c message.c
	gcc -c protocole.c
	gcc -c client_function.c
	gcc -c serv_function.c
	gcc -c annuaire.c
	gcc -c client.c
	gcc -c transfert.c
	gcc -c online.c
	gcc -Wall -g -o $@ message.o protocole.o client.o client_function.o serv_function.o transfert.o annuaire.o online.o -Wall -g -lpthread

