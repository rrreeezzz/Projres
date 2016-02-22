#include "utilities.c"
#include "server_function.h"
#include "db.h"

int main(int argc, char *argv[]) {

	time_server();

	handler_sigint();

	connect_mysql();

  server_sockfd = init_server();

	routine_server(server_sockfd);

	exit(EXIT_SUCCESS);
}//main
