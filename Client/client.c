#include "client_function.h"
#include "clientLib.h"

int main(int argc, char *argv[]) {

	int * server_sockfd;

	handler_sigint();

  server_sockfd = init_server();

	routine_server(server_sockfd);

	exit(EXIT_SUCCESS);
}//main
