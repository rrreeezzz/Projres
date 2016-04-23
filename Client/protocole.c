#include "protocole.h"

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

void rechercheProtocol(char *msg, int *client_sockfd, client_data *fd_array, int *num_clients, fd_set *readfds) { //les erreurs de cmd seront gérées côté client et la cmd help aussi !!!!!!!

	/*Fonction qui va, en fonction du type de message reçu, appliquer la bonne opération dessus*/
	static int file_fd = -1;
	int tab[2];
	char filename[256];

	message *msg_send = (message *) malloc(sizeof(message));
	message *msg_rcv = (message *) malloc(sizeof(message));
	//printf("%s\n",msg );

	if(protocol_parser(msg, msg_rcv) != -1) {

		/*Ecrire une fonction qui check quand le message a été envoyé
		et si trop vieux on fait pas le switch case*/

		/* On agis en fonctions du type de message */

		switch((*msg_rcv).code) {

			/*
			1** : Messages normaux
			*/

			// 100 : msg normal
			case 100:
			if (search_client_ready_by_fd(*client_sockfd, fd_array, num_clients) != -1){//on regarde si le client est ready (session-initiate et session_accept passées)
				/*A CHANGER, TEMPORAIRE*/
				printf("[%s] %s", fd_array[search_client_array_by_fd(*client_sockfd, fd_array, num_clients)].name_client, (*msg_rcv).msg_content);
			} else {
				printf("[PROGRAM] %s not ready for communication\n", fd_array[search_client_array_by_fd(*client_sockfd, fd_array, num_clients)].name_client);
			}
			break;

			// 101 : grp ?

			// 102 : données de transfert de fichier
			case 102:
			if(write(file_fd, msg_rcv->msg_content, msg_rcv->length)<0);
			// gestion erreur ?!!!!!!!!!! a faire
			break;

			/*
			2** : Processus d'identification
			*/

			// 200 : SESSION_INITIATE Si un client se connecte
			case 200:
			if(control_accept(msg_rcv, fd_array, num_clients) == 0) {
				//On lui demande de se logger
				if (login_client(msg_rcv, msg_send,client_sockfd, fd_array, num_clients, readfds) != -1) {
					//On confirme la connection du client
					session_accept(msg_send); //on crée le message de session-accept-1
					send_msg(msg_send, client_sockfd, readfds, fd_array, num_clients);
					free((*msg_send).msg_content);
				}
			} else {
				printf("[PROGRAM] Session not established : you refused the connection with %s.\n", (*msg_rcv).msg_content);
				session_denied(msg_send, 2);
				send_msg(msg_send, client_sockfd, readfds, fd_array, num_clients);
				free((*msg_send).msg_content);
				close(*client_sockfd);
				FD_CLR(*client_sockfd, readfds);
			}
			break;

			// 201 : SESSION_ACCEPT
			case 201:
			if (login_client(msg_rcv, msg_send, client_sockfd, fd_array, num_clients, readfds) != -1) {
				session_confirmed(msg_send); //on crée le message de session-accept-2
				send_msg(msg_send, client_sockfd, readfds, fd_array, num_clients);
				free((*msg_send).msg_content);
				client_ready(*client_sockfd, fd_array, num_clients);
				printf("You are now in communication with : %s\n\n", (*msg_rcv).msg_content);
			}
			break;

			// 202 : SESSION_CONFIRMED
			case 202:
			client_ready(*client_sockfd, fd_array, num_clients);
			printf("You are now in communication with : %s\n\n", (*msg_rcv).msg_content);
			break;

			// 203 : TRANSFER_INITIATE
			case 203:
			printf("TRANS INIT data : %s\n", msg_rcv->msg_content); // POUR DEBUG
			if((file_fd = ask_transfer(msg_rcv, filename)) < 0){
				transfer_refused(msg_send);
			} else {
				transfer_accept(msg_send, filename);
			}
			send_msg(msg_send,client_sockfd,readfds,fd_array,num_clients);
			free((*msg_send).msg_content);
			break;

			// 204 : TRANSFER_ACCEPT
			case 204:
			prepare_transfer(msg_rcv, *client_sockfd,readfds,fd_array,num_clients);
			break;


			/*
			3** : Gestion des erreurs
			*/

			// 300 : SESSION_DENIED
			case 300:
			printf("[%s] Connection denied : too many client online.\n", (*msg_rcv).msg_content);
			close(*client_sockfd);
			FD_CLR(*client_sockfd, readfds);
			break;

			// 301 : SESSION_DENIED
			case 301:
			printf("[%s] Connection denied : you are already in connection.\n", (*msg_rcv).msg_content);
			close(*client_sockfd);
			FD_CLR(*client_sockfd, readfds);
			break;

			// 302 : SESSION_DENIED
			case 302:
			printf("[%s] Connection denied : the user you tried to connect to refused the connection.\n", (*msg_rcv).msg_content);
			close(*client_sockfd);
			FD_CLR(*client_sockfd, readfds);
			break;

			// 303 : SESSION_END
			case 303:
			printf("[%s] End of connection.\n", (*msg_rcv).msg_content);
			exitClient(*client_sockfd, readfds, fd_array, num_clients);
			break;

			// 304 : TRANSFER_REFUSED
			case 304:
			printf("[%s] Transfer refused.\n", (*msg_rcv).msg_content);
			break;

			// 305 : TRANSFER_CANCELLED
			/*le client peut annuler le transfert, donc on stop le thread, {est ce que ça freee et ça close bien ?} */
			/* mais si le client plante et quitte sans envoyer de TRANSFER_CANCELLED, il se passe quoi pour le thread ??????????????? */

			// 306 : TRASNFER_ABORTED
			/* l'expéditeur a annulé, on ferme fd et on demande si le receveur veut garder le fichier */

			//307 : TRANSFER_END
			case 307:
			printf("Transfer of file %s succeed\n", (*msg_rcv).msg_content);
			close(file_fd);
			file_fd = -1;
			break;

			case 403:
			printf("[PROGRAM] : L'adresse de l'utilisateur recherché est : %s\n", (*msg_rcv).msg_content);
			break;

			case 411:
			printf("[PROGRAM] : L'adresse de l'utilisateur recherché n'existe pas dans la base de données.\n");
			break;

			default:
			break;
		}
	}
	free(msg_send);
	free(msg_rcv);
}
