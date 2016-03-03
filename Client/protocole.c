#include "protocole.h"

void send_msg(message *segment, int *fd){

	/*Fonction qui concatène et envois les trames*/

	(*segment).temps = time(NULL);
	(*segment).length = MSG_SIZE;
	char msg[(*segment).length];

	sprintf(msg, "%d/%d/%d/%s/END", (*segment).code, (*segment).length, (int) (*segment).temps, (*segment).msg_content);

	printf("msg envoyé : %s\n", msg);
	write(*fd, msg, MSG_SIZE); //gerer les erreurs ?

}

int protocol_parser(char *msg, message *msg_rcv){

	/*Fonction qui se charge de séparer les différents champs de la trame reçu*/

	char * sep = NULL;
	char code[3], length[MSG_SIZE], send_time[MSG_SIZE], data[MSG_SIZE];

	if(sscanf(msg, "%[^'/']/%[^'/']/%[^'/']/%[^'/]/END", code, length, send_time, data) == 4){
		(*msg_rcv).code = atoi(code);
		(*msg_rcv).length = atoi(length);
		(*msg_rcv).temps = (time_t) atoi(send_time);
		(*msg_rcv).msg_content = malloc(strlen(data)*sizeof(char));
		strcpy((*msg_rcv).msg_content,data);
		return 0;
	}
	/*
	if((sep = strchr(msg, '/')) != NULL){
		strcpy((*msg_rcv).msg_content, sep+1);
		return 0;
	}*/

	return -1;
}

void rechercheProtocol(char *msg, int *client_sockfd, client_data *fd_array, int *num_clients, fd_set *readfds) { //les erreurs de cmd seront gérées côté client et la cmd help aussi !!!!!!!

	/*Fonction qui va, en focntion du type de message reçu, appliquer la bonne opération dessus*/

	message * msg_rcv = (message *) malloc(sizeof(message));

	if(protocol_parser(msg, msg_rcv) != -1){

		switch((*msg_rcv).code){
			/*case 100:
				display_msg
				break;*/
			case 200:
			login_client((*msg_rcv).msg_content, client_sockfd, fd_array, num_clients, readfds);
			break;
			//case 201:
			default:
				break;
		}
	}
	free((*msg_rcv).msg_content);
	free(msg_rcv);
}
