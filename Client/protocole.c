void send_msg(message *segment, int *fd){

	char msg[(*segment).length];

	sprintf(msg, "%d/%d/%d/%s/END", (*segment).code, (*segment).length, (int) (*segment).temps, (*segment).msg_content);

	write(fd, msg, (*segment).length); //gerer les erreurs ?

}

int protocol_parser(char *msg, message *msg_rcv){

	char * sep = NULL;

  sscanf(msg, "%d/%d/%d/%s/END", &(*msg_rcv).code, &(*msg_rcv).length, (int) &(*msg_rcv).temps, (*msg_rcv).msg_content);

	if((sep = strchr(msg, '/')) != NULL){
		(*msg_rcv).msg_content = malloc(sizeof(sep+1));
		strcpy((*msg_rcv).msg_content, sep+1);
		return 0;
	}

	return -1;
}

void rechercheProtocol(char *msg, int *client_sockfd, client_data *fd_array, int *num_clients, fd_set *readfds) { //les erreurs de cmd seront gérées côté client et la cmd help aussi !!!!!!!

	message msg_rcv;

	sscanf(msg, "%d/", &msg_rcv.code);

	if(protocol_parser(msg, &msg_rcv) != -1){

		switch(msg_rcv.code){
			/*case 100:
				display_msg
				break;*/
			case 200:
			login_client(msg_rcv.msg_content, client_sockfd, fd_array, num_clients, readfds);
			break;
			//case 201:
			default:
				break;
		}
	free(msg_rcv.msg_content);
	}
}
