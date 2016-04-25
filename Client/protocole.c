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

void sendUiMsg(char * content, fd_set *readfds, client_data *fd_array, int *num_clients) {
	message *msg = (message *) malloc(sizeof(message));
	normal_msg(msg,content);
	send_msg(msg, &userInterface_fd ,readfds,fd_array,num_clients);
	free((*msg).msg_content);
	free(msg);
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

void rechercheProtocol(char *msg, int *client_sockfd, client_data *fd_array, int *num_clients, fd_set *readfds, waitList *waitlist) { //les erreurs de cmd seront gérées côté client et la cmd help aussi !!!!!!!

	/*Fonction qui va, en fonction du type de message reçu, appliquer la bonne opération dessus*/

	char buffer[50];
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
				//on avertis l'ui si elle est connectee
				if (userInterface_fd > 0 ) {
					char content[MSG_SIZE];
					sprintf(content,"MSGRECV %s %s\n",fd_array[search_client_array_by_fd(*client_sockfd, fd_array, num_clients)].name_client, (*msg_rcv).msg_content);
					sendUiMsg(content,readfds,fd_array,num_clients);
				}
			} else {
				printf(BLUE"[PROGRAM] "RED"%s"BLUE" not ready for communication"RESET"\n", fd_array[search_client_array_by_fd(*client_sockfd, fd_array, num_clients)].name_client);
			}
			break;

			// 101 : grp ?

			// 102 : données de transfert de fichier
			case 102:
			write(fd_array[search_client_array_by_fd(*client_sockfd, fd_array, num_clients)].fd_transfer, msg_rcv->msg_content, msg_rcv->length);
			// gestion erreur ?!!!!!!!!!! a faire
			break;

			/*
			2** : Processus d'identification
			*/

			// 200 : SESSION_INITIATE Si un client se connecte
			case 200:
			if (strcmp((*msg_rcv).msg_content,"USERINTERFACE\0")==0){
				printf(BLUE"\n[PROGRAM] A user interface at the adress "RED"%s"BLUE" is trying to establish a connection with you. It can control the program if you accept. Do you accept ? Type \"/accept "RED"%s"BLUE"\" or \"/refuse "RED"%s"BLUE"\"."RESET"\n", fd_array[*num_clients+(*waitlist).nb_connect].address_client, (*msg_rcv).msg_content, (*msg_rcv).msg_content);
			} else {
				printf(BLUE"\n[PROGRAM] "RED"%s : %s"BLUE" is trying to establish a connection with you. Do you accept ? Type \"/accept "RED"%s"BLUE"\" or \"/refuse "RED"%s"BLUE"\"."RESET"\n", (*msg_rcv).msg_content, fd_array[*num_clients+(*waitlist).nb_connect].address_client, (*msg_rcv).msg_content, (*msg_rcv).msg_content);

				//on avertis l'ui si elle est connectee
				if (userInterface_fd > 0 ) {
					char content[MSG_SIZE];
					sprintf(content,"CONNECTIONASK %s %s\n",(*msg_rcv).msg_content, fd_array[*num_clients+(*waitlist).nb_connect].address_client);
					sendUiMsg(content,readfds,fd_array,num_clients);
				}

			}
			fd_array[*num_clients+(*waitlist).nb_connect].fd_client = *client_sockfd;
			strcpy(fd_array[*num_clients+(*waitlist).nb_connect].name_client, (*msg_rcv).msg_content);
			break;

			// 201 : SESSION_ACCEPT
			case 201:
			strcpy(fd_array[*num_clients+(*waitlist).nb_connect].name_client, (*msg_rcv).msg_content);
			if (login_client(msg_send, client_sockfd, fd_array, num_clients, readfds, waitlist) != -1) {
				session_confirmed(msg_send); //on crée le message de session-accept-2
				send_msg(msg_send, client_sockfd, readfds, fd_array, num_clients);
				free((*msg_send).msg_content);
				client_ready(*client_sockfd, fd_array, num_clients);
				if (strcmp((*msg_rcv).msg_content,"USERINTERFACE\0")==0){
					printf(BLUE "A user interface is now connected, you will see all its activities from here."RESET"\n\n");
					userInterface_fd=*client_sockfd;
				} else {
					printf(BLUE "You are now in communication with : "RED"%s" RESET "\n\n", (*msg_rcv).msg_content);

					//on avertis l'ui si elle est connectee
					if (userInterface_fd > 0 ) {
						char content[MSG_SIZE];
						sprintf(content,"CONNECTIONCONFIRM %s\n",(*msg_rcv).msg_content);
						sendUiMsg(content,readfds,fd_array,num_clients);
					}
				}
			}
			break;

			// 202 : SESSION_CONFIRMED
			case 202:
			client_ready(*client_sockfd, fd_array, num_clients);
			if (strcmp((*msg_rcv).msg_content,"USERINTERFACE\0")==0){
				printf(BLUE "A user interface is now connected, you will see all its activities from here."RESET"\n\n");
				userInterface_fd=*client_sockfd;
			} else {
				printf(BLUE "You are now in communication with : "RED"%s" RESET "\n\n", (*msg_rcv).msg_content);

				//on avertis l'ui si elle est connectee
				if (userInterface_fd > 0 ) {
					char content[MSG_SIZE];
					sprintf(content,"CONNECTIONCONFIRM %s\n",(*msg_rcv).msg_content);
					sendUiMsg(content,readfds,fd_array,num_clients);
				}
			}
			break;

    	// 203 : TRANSFER_INITIATE
      case 203:
	  	if((fd_array[search_client_array_by_fd(*client_sockfd, fd_array, num_clients)].fd_transfer = ask_transfer(msg_rcv, filename)) < 0){
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
			printf(BLUE"["RED"%s"BLUE"] Connection denied : too many client online."RESET"\n", (*msg_rcv).msg_content);
			close(*client_sockfd);
			FD_CLR(*client_sockfd, readfds);
			break;

			// 301 : SESSION_DENIED
			case 301:
			printf(BLUE"["RED"%s"BLUE"] Connection denied : you are already in connection."RESET"\n", (*msg_rcv).msg_content);
			close(*client_sockfd);
			FD_CLR(*client_sockfd, readfds);
			break;

			// 302 : SESSION_DENIED
			case 302:
			printf(BLUE"[%s] Connection denied : the user you tried to connect to refused the connection."RESET"\n", (*msg_rcv).msg_content);
			close(*client_sockfd);
			FD_CLR(*client_sockfd, readfds);
			break;

			// 303 : SESSION_END
			case 303:
			exitClient(*client_sockfd, readfds, fd_array, num_clients);
			break;

			// 304 : SESSION_ABORTED
			case 304:
			printf(BLUE"["RED"%s"BLUE"] User aborted the connection."RESET"\n", (*msg_rcv).msg_content);
			close(*client_sockfd);
			FD_CLR(*client_sockfd, readfds);
			//Quelque chose à enlever ? exitClient(*client_sockfd, readfds, fd_array, num_clients);
			break;

	    // 305 : TRANSFER_REFUSED
			case 305:
			printf(BLUE"["RED"%s"BLUE"] Transfer refused."RESET"\n", (*msg_rcv).msg_content);
			break;

			// 306 : TRANSFER_CANCELLED
			/*le client peut annuler le transfert, donc on stop le thread, {est ce que ça freee et ça close bien ?} */
			/* mais si le client plante et quitte sans envoyer de TRANSFER_CANCELLED, il se passe quoi pour le thread ??????????????? */

			// 307 : TRASNFER_ABORTED
			/*case 307:
				printf("[%s] Transfer aborted.\n", (*msg_rcv).msg_content);
				printf("[PROGRAM] Do you still want to keep the file ? Type y or n\n");
				fgets(res, 4, stdin);
				if ((strcmp(res, "yes\n")==0) || (strcmp(res, "y\n")==0)) {
					break;
				}
				else {

				}*/
			/* l'expéditeur a annulé, on ferme fd et on demande si le receveur veut garder le fichier */

			//308 : TRANSFER_END
			case 308:
			printf(BLUE"Transfer of file "RED"%s"BLUE" succeed"RESET"\n", (*msg_rcv).msg_content);
			close(fd_array[search_client_array_by_fd(*client_sockfd, fd_array, num_clients)].fd_transfer);
			break;

			case 403:
			printf(BLUE"[PROGRAM] : L'adresse de l'utilisateur recherché est : "RED"%s"RESET"\n", (*msg_rcv).msg_content);
			break;

			case 411:
			printf(BLUE"[PROGRAM] : L'adresse de l'utilisateur recherché n'existe pas dans la base de données."RESET"\n");
			break;

			#if defined(PROJ)
			case 500:
			sprintf(buffer,"vocal_%s.wav", msg_rcv->msg_content);
			fd_array[search_client_array_by_fd(*client_sockfd, fd_array, num_clients)].fd_vocal = open(buffer, O_CREAT | O_TRUNC | O_WRONLY, 0755);
			break;

			case 501:
			write(fd_array[search_client_array_by_fd(*client_sockfd, fd_array, num_clients)].fd_vocal, msg_rcv->msg_content, msg_rcv->length);
			break;

			case 502:
            		sprintf(buffer,"vocal_%s.wav", msg_rcv->msg_content);
			printf("[%s] VOCAL MESSAGE\n", fd_array[search_client_array_by_fd(*client_sockfd, fd_array, num_clients)].name_client);
			close(fd_array[search_client_array_by_fd(*client_sockfd, fd_array, num_clients)].fd_vocal);
			main_lecture(buffer);
			break;
			#endif

			default:
			break;
		}
	}
	free(msg_rcv->msg_content);
	free(msg_send);
	free(msg_rcv);
}
