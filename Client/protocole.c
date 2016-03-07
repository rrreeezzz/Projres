#include "protocole.h"

void send_msg(message *segment, int *fd) {

	/*Fonction qui concatène et envois les trames*/

	(*segment).temps = time(NULL);
	(*segment).length = strlen((*segment).msg_content);
	char msg[MSG_SIZE];

	sprintf(msg, "%d/%d/%d/%s/END", (*segment).code, (*segment).length, (int) (*segment).temps, (*segment).msg_content);

	printf("msg envoyé : %s\n", msg);
	write(*fd, msg, strlen(msg)); //avec strlen pas de bug, bizarre... changer avec la bonne taille
}

int protocol_parser(char *msg, message *msg_rcv) {

	/*Fonction qui se charge de séparer les différents champs de la trame reçu*/

	char * sep = NULL;
	char code[3], length[MSG_SIZE], send_time[MSG_SIZE], data[MSG_SIZE];
	(*msg_rcv).msg_content = malloc(MSG_SIZE);

	if(sscanf(msg, "%[^'/']/%[^'/']/%[^'/']/%[^'/]/END", code, length, send_time, data) == 4){
		(*msg_rcv).code = atoi(code);
		(*msg_rcv).length = atoi(length);
		(*msg_rcv).temps = (time_t) atoi(send_time);
		memcpy((*msg_rcv).msg_content, data, (*msg_rcv).length);
		return 0;
	}

	return -1;
}

void rechercheProtocol(char *msg, int *client_sockfd, client_data *fd_array, int *num_clients, fd_set *readfds) { //les erreurs de cmd seront gérées côté client et la cmd help aussi !!!!!!!

	/*Fonction qui va, en fonction du type de message reçu, appliquer la bonne opération dessus*/
	message * msg_rcv = (message *) malloc(sizeof(message));
	message * msg_send = (message *) malloc(sizeof(message));

	if(protocol_parser(msg, msg_rcv) != -1){

		/*Ecrire une fonction qui check quand le message a été envoyé
		et si trop vieux on fait pas le switch case*/

		/* On agis en fonctions du type de message */
		switch((*msg_rcv).code){

			/*
				1** : Messages normaux
			*/

			// 100: accepter la connection
			case 100:
				if (search_client_ready_by_fd(*client_sockfd, fd_array, num_clients) != -1){//on regarde si le client est ready (session-initiate et session_accept passées)
					/* à faire*/
				}else{
					printf("Client not ready for communication\n");
				}
				break;

			/*
				2** : Processus d'identification
			*/

			// 200 : SESSION_INITIATE
			case 200:
				if (login_client(msg_rcv, msg_send,client_sockfd, fd_array, num_clients, readfds) != -1) {
					session_accept(msg_send); //on créer le message de session-accept-1
					send_msg(msg_send,client_sockfd);
				}
				break;

			// 201 : SESSION_ACCEPT
			case 201:
				if (login_client(msg_rcv, msg_send, client_sockfd, fd_array, num_clients, readfds) != -1) {
					session_confirmed(msg_send); //on créer le message de session-accept-2
					send_msg(msg_send, client_sockfd);
					client_ready(*client_sockfd, fd_array, num_clients);
					printf("You are now in communication with : %s\n", (*msg_rcv).msg_content+5);
				}
				break;

			// 202 : SESSION_CONFIRMED
			case 202:
				client_ready(*client_sockfd, fd_array, num_clients);
				printf("You are now in communication with : %s\n", (*msg_rcv).msg_content+5);
				break;

			/*
				3** : Gestion des erreurs
			*/

			// 300 : SESSION_DENIED
			case 300:
				break;

			// 301 : SESSION_DENIED
			case 301:
				printf("%s\n", (*msg_rcv).msg_content);
				close(*client_sockfd);
				FD_CLR(*client_sockfd, readfds);
				break;
				
			// 302 : SESSION_END
			case 302:
				printf("%s\n", (*msg_rcv).msg_content);
				exitClient(*client_sockfd, readfds, fd_array, num_clients);
				default:
			break;
		}

		free((*msg_rcv).msg_content);
		free((*msg_send).msg_content);
	}

	free(msg_send);
	free(msg_rcv);
}
