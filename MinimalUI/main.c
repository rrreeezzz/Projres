/*
*  Client Minimal pour controller le serveur depuis une UI
*  Cette UI est tout simplement un client console qui utilise le protocole d'echange du client principal
*  pour communiquer avec le protocole applicatif
*/

#include "main.h"
#include "utilities.h"

/*
* Messages possibles
*/
//message normal
void normal_msg(message *segment, char *data) {
	/*Fonction qui permet d'envoyer un message*/
	(*segment).code = 100;
	(*segment).msg_content = (char *) malloc(WRITE_SIZE);
	sprintf((*segment).msg_content, "%s", data);
}
//deconnection
void session_end(message *segment) {
	/*Fonction pour envoyer fin de connection*/
	(*segment).code = 303;
	(*segment).msg_content = (char *) malloc(WRITE_SIZE);
	sprintf((*segment).msg_content, "%s", General_Name);
	(*segment).length = strlen((*segment).msg_content);
}


/*
* Envoi de donnees
*/
void send_msg(message *segment, int *fd, fd_set *readfds, client_data *fd_array, int *num_clients) {

	/*Fonction qui concatène et envois les trames*/

	(*segment).temps = time(NULL);
	char msg[MSG_SIZE];

	sprintf(msg, "%d/%d/%d/", (*segment).code, (*segment).length, (int) (*segment).temps);
	//printf("msg envoyé : %s\n", msg); //pour debug
	memcpy(msg+(strlen(msg)), (*segment).msg_content, WRITE_SIZE);  //50

	if (write(*fd, msg, MSG_SIZE) <= 0) {
		perror("Write error");
		exitClient(*fd, readfds, fd_array, num_clients);
	}
}

/*
* Parseur
*/
int protocol_parser(char *msg, message *msg_rcv) {

	/*Fonction qui se charge de séparer les différents champs de la trame reçu*/

	char * sep = NULL;
	char code[3], length[WRITE_SIZE], send_time[WRITE_SIZE];
	(*msg_rcv).msg_content = malloc(WRITE_SIZE);

	if(sscanf(msg, "%[^/]/%[^/]/%[^/]/", code, length, send_time) == 3){
		(*msg_rcv).code = atoi(code);
		(*msg_rcv).length = atoi(length);
		(*msg_rcv).temps = (time_t) atoi(send_time);
		sep=strchr(msg,'/'); //
		sep=strchr(sep+1,'/'); //
		sep=strchr(sep+1,'/'); // Pour se mettre après le dernier / pour memcpy le content
		memcpy((*msg_rcv).msg_content, sep+1, WRITE_SIZE); //50
		return 0;
	}
	return -1;
}

void routine_client(int fdClient){

	struct sockaddr_in client_address;
	int addresslen = sizeof(struct sockaddr_in);
	client_data fd_array[1]; //tableau de data client
	fd_set readfds, testfds;
	int maxfds;
	char client_inaddr[INET_ADDRSTRLEN];
	message *msg = (message *) malloc(sizeof(message));

	FD_ZERO(&readfds);
	FD_SET(*server_sockfd, &readfds);
	FD_SET(0, &readfds);  /* On ajoute le clavier au file descriptor set */

  //Connection au client
  int sock_host;
  struct hostent *hostinfo;
  struct sockaddr_in address;

  sock_host = socket(AF_INET, SOCK_STREAM, 0);

  address.sin_addr.s_addr = inet_addr(adresseClientPrincipal);
  address.sin_family = AF_INET;
  address.sin_port = htons(portClientPrincipal);

  //Tentative de connection
  if (connect(sock_host, (struct sockaddr *)&address, sizeof(address)) < 0){
    return -1;
  }

  //Recuperation du nom

  //Ajout au fd_read

	/*  Attente de requetes */
	while (*server_sockfd != -1) {
		testfds = readfds;
		//printf("maxfds : %i\n", maxfds); //debug
		if(select(maxfds+1, &testfds, NULL, NULL, NULL) < 0) {
			perror("Select");
			exit(EXIT_FAILURE);
		}

		/* Il y a une activité, on cherche sur quel descripteur grâce à FD_ISSET */
		for (fd=0; fd<maxfds+1; fd++) {
			if (FD_ISSET(fd, &testfds)) {
				if (fd == 0) {  //Activite sur le clavier



				} else if (fdClient == fd) {  // Retour de commandes




        }//if fd ==
	    }//if FD_ISSET
	  }//for
	}//while
}


int main(int argc, char *argv[]) {
	int fdClient;

  adresseClientPrincipal = "127.0.0.1";
  portClientPrincipal = "40190";

  if ( (fdClient = connectClient()) > 0){
    routine_server(fdClient);
  } else {
    printf("Impossible de se connecter.\n");
  }


	exit(EXIT_SUCCESS);
}
