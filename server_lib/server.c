#include "server_function.h"
#include "db.h"

int main(int argc, char *argv[]) {

	//time_server();

	//handler_sigint();

	connect_mysql(); //initialisation base de données

  init_server();

	routine_server();

	exit(EXIT_SUCCESS);
}//main
