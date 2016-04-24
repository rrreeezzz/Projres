/*
*  Client Minimal pour controller le serveur depuis une UI
*  Cette UI est tout simplement un client console qui utilise le protocole d'echange du client principal
*  pour communiquer avec le protocole applicatif
*/

#include "main.h"
#include "utilities.h"
#include "message.h"
#include "protocol.h"
#include "basicCommunication.h"


int main(int argc, char *argv[]) {

  strcpy(adresseClientPrincipal,"127.0.0.1");
  portClientPrincipal = 55067;

	if (connect_client() < 0){
		perror("Impossible to reach the server");
		exit(EXIT_FAILURE);
	} else {
		printf(BLUE"Connection Succes!\n"RESET);
	}



  while ( routine_client() == 0) {}

	exit(EXIT_SUCCESS);
}
