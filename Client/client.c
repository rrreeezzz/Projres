#include "serv_function.h"
#include "client_function.h"

int main(int argc, char *argv[]) {
	int * server_sockfd;

	ask_name();

	handler_sigint();

    server_sockfd = init_server();

	routine_server(server_sockfd);

	exit(EXIT_SUCCESS);
}//main
