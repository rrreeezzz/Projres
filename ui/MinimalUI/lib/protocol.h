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

//Protocole minimal
int rechercheProtocol(char *msg) {
	message *msg_rcv = (message *) malloc(sizeof(message));

	if(protocol_parser(msg, msg_rcv) != -1) {
		switch((*msg_rcv).code) {

			// 100 : msg normal
			case 100:
        parseApplicationMessage((*msg_rcv).msg_content);
				break;

			// 303 : SESSION_END
			case 303:
				close(fdClientPrincipal);
				free(msg_rcv->msg_content);
				free(msg_rcv);
				return 1; //deconnection normale
				break;

			default:
				break;
		}
	}
	free(msg_rcv->msg_content);
	free(msg_rcv);
  return 0;
}
