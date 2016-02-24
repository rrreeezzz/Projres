#include "client_function.c"
#include "clientLib.c"

int main(int argc, char *argv[]) {

	int * server_sockfd;

	handler_sigint();

  server_sockfd = init_server();

	routine_server(server_sockfd);

	exit(EXIT_SUCCESS);
}//main
