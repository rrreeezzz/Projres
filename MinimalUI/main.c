/*
*  Client Minimal pour controller le serveur depuis une UI
*  Cette UI est tout simplement un client console qui utilise le protocole d'echange du client principal
*  pour communiquer avec le protocole applicatif
*/

#include "main.h"
#include "utilities.h"
#include "message.h"
#include "protocol.h"

/*
* Envoi de donnees
*/
void send_msg(message *segment) {
	(*segment).temps = time(NULL);
	char msg[MSG_SIZE];

	sprintf(msg, "%d/%d/%d/", (*segment).code, (*segment).length, (int) (*segment).temps);
	memcpy(msg+(strlen(msg)), (*segment).msg_content, WRITE_SIZE);

	if (write(fdClientPrincipal, msg, MSG_SIZE) <= 0) {
		perror("Write error");
		close(fdClientPrincipal);
		exit(EXIT_FAILURE);
	}
}

/*
* Envoi d'un message normal
*/
void sendRequest(char * content) {
	message *msg = (message *) malloc(sizeof(message));
	normal_msg(msg,content);
	send_msg(msg);
	free((*msg).msg_content);
	free(msg);
}

void traiterRequete() {

	char msg[MSG_SIZE];
	int  result;

	memset(msg, '\0', MSG_SIZE);

	if ((result = read(fdClientPrincipal, msg, MSG_SIZE)) > 0) { /* Une requête en attente sur le descripteur fd */
		//printf("msg recu : %s\n", msg); //pour debug
		rechercheProtocol(msg);
	} else {
		close(fdClientPrincipal);
		exit(EXIT_FAILURE);
	}

}

int routine_client(){

	fd_set readfds, testfds;
	message *mesge = (message *) malloc(sizeof(message));
	char msg[WRITE_SIZE];
	struct sockaddr_in address;

	FD_ZERO(&readfds);
	FD_SET(0, &readfds);  /* On ajoute le clavier au file descriptor set */

  //Connection au client
  fdClientPrincipal = socket(AF_INET, SOCK_STREAM, 0);

  address.sin_addr.s_addr = inet_addr(adresseClientPrincipal);
  address.sin_family = AF_INET;
  address.sin_port = htons(portClientPrincipal);

  //Tentative de connection
  if (connect(fdClientPrincipal, (struct sockaddr *)&address, sizeof(address)) < 0){
    return -1;
  }

	//Envoi du nom dans un message session-initiate
	session_initiate(mesge);
	send_msg(mesge);
	free((*mesge).msg_content);

  //Recuperation du nom, et de la confrirmation de connection
	if (read(fdClientPrincipal, msg, MSG_SIZE) < 0){
		return -1;
	}
	protocol_parser(msg,mesge);
	if (mesge->code != 201){
		return -1;
	}
	strncpy(General_Name,(*mesge).msg_content,strlen((*mesge).msg_content));
	free((*mesge).msg_content);

	//confirmation de la connection
	session_confirmed(mesge); //on crée le message de session-accept-2
	send_msg(mesge);
	free((*mesge).msg_content);


  //Ajout au readfds
	FD_SET(fdClientPrincipal, &readfds);

	/*  Attente de requetes */
	while (1) {
		testfds = readfds;
		//printf("maxfds : %i\n", maxfds); //debug
		if(select(fdClientPrincipal+1, &testfds, NULL, NULL, NULL) < 0) {
			perror("Select");
			exit(EXIT_FAILURE);
		}

		if (FD_ISSET(0, &testfds)) {
			fgets(msg, WRITE_SIZE, stdin);
			sendRequest(msg);
		}
		if (FD_ISSET(fdClientPrincipal, &testfds)) {
			traiterRequete();
		}
	}//while
}


int main(int argc, char *argv[]) {

  strcpy(adresseClientPrincipal,"127.0.0.1");
  portClientPrincipal = 54942;

  routine_client();

	exit(EXIT_SUCCESS);
}
