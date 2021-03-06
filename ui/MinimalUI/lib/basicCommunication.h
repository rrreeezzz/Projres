/*
* Envoi de donnees
*/
int send_msg(message *segment) {
	(*segment).temps = time(NULL);
	char msg[MSG_SIZE];

	sprintf(msg, "%d/%d/%d/", (*segment).code, (*segment).length, (int) (*segment).temps);
	memcpy(msg+(strlen(msg)), (*segment).msg_content, WRITE_SIZE);

	if (write(fdClientPrincipal, msg, MSG_SIZE) <= 0) {
		perror("Write error");
		close(fdClientPrincipal);
		return -1;
	}
  return 0;
}

/*
* Envoi d'un message de fin de session
*/
int sendSessionEnd(){
  int result;
	message *msg = (message *) malloc(sizeof(message));

	session_end(msg);
	result = send_msg(msg);

	free((*msg).msg_content);
	free(msg);

  return result;
}

/*
* Envoi d'un message normal
*/
int sendRequest(char * content) {
  int result;
	message *msg = (message *) malloc(sizeof(message));

	normal_msg(msg,content);
	result = send_msg(msg);

	free((*msg).msg_content);
	free(msg);

  return result;
}

/*
* Envoi d'un message normal
*/
int sendInitiate() {
  int result;
	message *msg = (message *) malloc(sizeof(message));

	session_initiate(msg);
	result = send_msg(msg);

	free((*msg).msg_content);
	free(msg);

  return result;
}

/*
* Envoi d'un message normal
*/
int sendConfirmation() {
  int result;
	message *msg = (message *) malloc(sizeof(message));

	session_confirmed(msg);
	result = send_msg(msg);

	free((*msg).msg_content);
	free(msg);

  return result;
}

int traiterRequete() {
	char msg[MSG_SIZE];
	int result;

	memset(msg, '\0', MSG_SIZE);

	if ((result = read(fdClientPrincipal, msg, MSG_SIZE)) > 0) { /* Une requête en attente sur le descripteur fd */
		//printf("msg recu : %s\n", msg); //pour debug
		return rechercheProtocol(msg);
	} else {
		close(fdClientPrincipal);
		return -1;
	}

}

int recvName(){
	message mesge;
	char msg[MSG_SIZE];

	//Recuperation du nom, et de la confrirmation de connection
	if (read(fdClientPrincipal, msg, MSG_SIZE) < 0){
		printf("Read error\n" );
		return -1;
	}
	protocol_parser(msg,&mesge);
	if (mesge.code != 201){
		free(mesge.msg_content);
		return -1;
	}
	strncpy(General_Name,mesge.msg_content,mesge.length);
	free(mesge.msg_content);
	return 0;
}

int connect_client(int stdincdt){
  	struct sockaddr_in address;

    //Creation socket client
    fdClientPrincipal = socket(AF_INET, SOCK_STREAM, 0);

    //Creation de l'extremite
    address.sin_addr.s_addr = inet_addr(adresseClientPrincipal);
    address.sin_family = AF_INET;
    address.sin_port = htons(portClientPrincipal);

    //Tentative de connection
    if (connect(fdClientPrincipal, (struct sockaddr *)&address, sizeof(address)) < 0){
      printf("Connection error\n" );
      return -1;
    }

  	//Envoi de session-initiate
  	if (sendInitiate() < 0){
      printf("Initiation Error\n" );
  		return -1;
    }

		if (recvName() < 0){
      printf("Acceptation error\n" );
      return -1;
    }

  	//confirmation de la connection
    if (sendConfirmation() < 0){
      printf("Confirmation Error\n" );
  		return -1;
    }

    FD_ZERO(&readfds);
		if (stdincdt == 1){
			FD_SET(0,&readfds);
		}
    FD_SET(fdClientPrincipal,&readfds);


    return 0;
}

/*
* Une boucle de la routine du serveur
* Valeurs de retour :
* -1 => erreur, deconnection brutale
* 0 => deroulement normal
* 1 => deconnection normale
*/
int routine_client(int stdincdt){
  fd_set testfds;
	char msg[WRITE_SIZE];

	//Structure de timeout
	struct timeval timeoutSelect;
	timeoutSelect.tv_usec = 10; //10 microsecondes
	timeoutSelect.tv_sec = 0;
	testfds = readfds;

  //On regarde l'activitee des FD
	if(select(fdClientPrincipal+1, &testfds, NULL, NULL, &timeoutSelect) < 0) {
		perror("Select");
		exit(EXIT_FAILURE);
		return 0;
	}
	if (stdincdt == 1){
		if (FD_ISSET(0, &testfds)) {
			fgets(msg, WRITE_SIZE, stdin);
			sendRequest(msg);
		}
	}

  //Retour de l'ui
	if (FD_ISSET(fdClientPrincipal, &testfds)) {
		traiterRequete();
	}

  return 0;
}
