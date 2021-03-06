/*
*  Client Minimal pour controller le serveur depuis une UI
*  Cette UI est tout simplement un client console qui utilise le protocole d'echange du client principal
*  pour communiquer avec le protocole applicatif
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

// Fonctions.h contient les differentes fonctions lies aux evenements du serveur
#include "fonctions.h"

// Librairie
#include "lib/minimalUI.h"

void handler_sigint(){

	/*Mise en plase du handler pour SIGINT*/

    printf(GREEN"\nExit minimal UI"RESET"\n");
    sendSessionEnd();
    exit(EXIT_SUCCESS);

}

int main(int argc, char *argv[]) {

	signal(SIGINT, handler_sigint);

	//On rentre l'adresse et le port
  strcpy(adresseClientPrincipal,"127.0.0.1");

  printf("On which port you want to connect ?\n");
  scanf("%d", &portClientPrincipal);
  //portClientPrincipal = 55567;


	//On tente de se connecter
	if (connect_client(MINIMALUI_STDIN) < 0){
		perror("Impossible to reach the server");
		exit(EXIT_FAILURE);
	} else {
		printf("Connection Succes!\n");
	}

	//On execute la routine du client
  while ( routine_client(MINIMALUI_STDIN) == 0) {}

	exit(EXIT_SUCCESS);
}
