#include "serv_function.h"
#include "client_function.h"

int main(int argc, char *argv[]) {
	int * server_sockfd;

	ask_name();

	handler_sigint();

  server_sockfd = init_server();

	routine_server(server_sockfd);

	//Suppression des potentiels .wav dans Client, suite a l'utilisation du vocal
	system("rm -f Client/*.wav");
	exit(EXIT_SUCCESS);
}//main
