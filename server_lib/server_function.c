#include "server_function.h"

void quit_server(){
	close(server_sockfd);
	exit_mysql();
	exit(0);
}

void handler_sigint(){

	/*Mise en plase du handler pour SIGINT*/
	/*On l'ignore car impossible de passer arguments a handler, sauf en passant par des variables globales*/
	/*On ne peut par contre pas intercepter SIGSTOP (ctrl-z) ou SIGKILL*/
	struct sigaction handler;
	memset(&handler, 0, sizeof(handler));
	handler.sa_handler = SIG_IGN;
	handler.sa_flags = 0;
	sigemptyset(&(handler.sa_mask));
	if(sigaction(SIGINT, &handler, NULL) != 0){
		perror("Error with sigaction");
		exit(EXIT_FAILURE);
	}

}

void init_server(){

	struct sockaddr_in server_address;
	int addresslen = sizeof(struct sockaddr_in);
	int server_sockfd;

	/*Récupération configuration*/
	char conf[MAX_SIZE_PARAMETER] = "port";
	if(get_Config(conf) < 0){
		perror("Erreur configuration port.");
		exit(EXIT_FAILURE);
	}
	int port = atoi(conf);


	printf("\n*** Server program starting (enter \"quit\" to stop) ***\n");

	server_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(port);
	if (bind(server_sockfd, (struct sockaddr *)&server_address, addresslen) < 0) { perror("Bind error."); exit(EXIT_FAILURE); }
	//if (getsockname(server_sockfd, (struct sockaddr *)&server_address, (socklen_t *)&addresslen) < 0) { perror("Getsockname error."); exit(EXIT_FAILURE); }

	printf("\n*** Connections available on port : %d ***\n", ntohs(server_address.sin_port));
}

void routine_server(){

	int on = 1;
	char buffer[INFO_SIZE];
	struct sockaddr remote_addr;
	fd_set readfds, tempfds;
	int fd;
	int maxfds;

	FD_ZERO(&readfds);
	FD_SET(server_sockfd, &readfds);
	FD_SET(0, &readfds);  /* On ajoute le clavier au file descriptor set */
	maxfds = server_sockfd;

	while(on){

		tempfds = readfds;

		printf("\e[1;33m[SERVER]\e[0m Waiting request...\n");

		if(select(maxfds+1, &tempfds, NULL, NULL, NULL) < 0) {
			perror("Select error.");
			exit(EXIT_FAILURE);
		}

		for(fd=0; fd<maxfds+1; fd++){

			if(FD_ISSET(fd, &tempfds)){

				if(fd == server_sockfd){
					if(recvfrom(server_sockfd, buffer, INFO_SIZE, 0, &remote_addr,(socklen_t *) sizeof(remote_addr)) > 0) //si on a rien lu, on retourne au while
						traiter_requete(buffer, remote_addr);

				}if(fd == 0){
					host_cmde();
				}
			} //if fd_isset
		} // for
	} // while

}

void host_cmde(){

	/* Fonction qui gère les commandes de l'admin du serveur.*/

	char commande[INFO_SIZE];

	fgets(commande, INFO_SIZE, stdin);
	//printf("commande : %s\n", commande);
	if(strncmp(commande, "/quit", 5) == 0)
	quit_server();
	else
	printf("\e[1;33m[SERVER]\e[0m Commande inconnue\n.");
}

void traiter_requete(char *buffer, struct sockaddr remote_addr){

	message *msg_rcv = (message *) malloc(sizeof(message));
	message *msg_send = (message *) malloc(sizeof(message));

	if(protocol_parser(buffer, msg_rcv) != -1) {

		/*Ecrire une fonction qui check quand le message a été envoyé
		et si trop vieux on fait pas le switch case*/

		/* On agis en fonctions du type de message */

		switch((*msg_rcv).code) {

			/* Discussion avec le serveur annuaire: 4XX */

			case 400: //Pour savoir si le serveur est ON
				if(add_user_mysql((*msg_rcv).msg_content) < 0){
					/*Si le pseudo est déjà pris*/
					already_exist(msg_send);
					send_msg(msg_send, remote_addr);
					free((*msg_send).msg_content);
					break;
				}
				im_on(msg_send);
				send_msg(msg_send, remote_addr);
				free((*msg_send).msg_content);
				break;

			case 402: //Pour savoir si untel est en ligne, on renvoi son IP si oui.
				if(exist_user_mysql((*msg_rcv).msg_content)){
					send_ip(msg_rcv, msg_send);
					send_msg(msg_send, remote_addr);
					free((*msg_send).msg_content);
					break;
				}
				no_exist(msg_send);
				send_msg(msg_send, remote_addr);
				free((*msg_send).msg_content);
				break;

			default:
			break;
		}
	}
	free(msg_send);
	free(msg_rcv);
}
