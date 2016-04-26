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
		perror("\e[1;33m[SERVER]\e[0m Error with sigaction");
		exit(EXIT_FAILURE);
	}

}

void init_server(){

	struct sockaddr_in server_address;
	int addresslen = sizeof(struct sockaddr_in);
	int optval = 1;

	/*Récupération configuration*/
	char conf[MAX_SIZE_PARAMETER] = "port";
	if(get_Config(conf) < 0){
		perror("\e[1;33m[SERVER]\e[0m Erreur configuration port.");
		exit(EXIT_FAILURE);
	}
	int port = atoi(conf);


	printf("\n\e[1;33m[SERVER]\e[0m Server program starting (enter \"/quit\" to stop)\n");

	if((server_sockfd = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
	perror("\e[1;33m[SERVER]\e[0m Erreur socket serveur.");

	// Pour réutiliser le port à la fin du programme
	if(setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0){
		perror("setsockopt() error: Re-use address");
		close(server_sockfd);
		exit(EXIT_FAILURE);
	}

	if(setsockopt(server_sockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval)) < 0) {
		perror("setsockopt() error: Keep Alive");
		close(server_sockfd);
		exit(EXIT_FAILURE);
	}

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(port);
	if (bind(server_sockfd, (struct sockaddr *)&server_address, addresslen) < 0) { perror("Bind error."); exit(EXIT_FAILURE); }
	//if (getsockname(server_sockfd, (struct sockaddr *)&server_address, (socklen_t *)&addresslen) < 0) { perror("Getsockname error."); exit(EXIT_FAILURE); }

	printf("\e[1;33m[SERVER]\e[0m Connections available on port %d and address %s\n", ntohs(server_address.sin_port), inet_ntoa(server_address.sin_addr));
}

void routine_server(){

	int on = 1;
	fd_set readfds, tempfds;
	int fd;
	int maxfds;

	FD_ZERO(&readfds);
	FD_SET(server_sockfd, &readfds);
	FD_SET(0, &readfds);  /* On ajoute le clavier au file descriptor set */
	maxfds = server_sockfd;

	if(listen(server_sockfd, 1) < 0) { perror("listen"); exit(EXIT_FAILURE); } // mettre plus que 1 utile ???

	while(on){

		tempfds = readfds;

		printf("\e[1;33m[SERVER]\e[0m Waiting request...\n");

		if(select(maxfds+1, &tempfds, NULL, NULL, NULL) < 0) {
			perror("\e[1;33m[SERVER]\e[0m Select error.");
			exit(EXIT_FAILURE);
		}

		for(fd=0; fd<maxfds+1; fd++){

			if(FD_ISSET(fd, &tempfds)){

				if(fd == server_sockfd){

					struct sockaddr_in client_address;
					socklen_t len = sizeof(client_address);
					int client_sockfd;

					if((client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, &len )) < 0 ) {
						perror("Accept");
						exit(EXIT_FAILURE);
					}

					char buffer[INFO_SIZE];

					if(read(client_sockfd, buffer, INFO_SIZE) <= 0){ //si on a rien lu, on retourne au while
						perror("\e[1;33m[SERVER]\e[0m Erreur read ");
					}else{
						traiter_requete(buffer, client_sockfd);
					}
				close(client_sockfd);

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
	printf("\e[1;33m[SERVER]\e[0m Commande inconnue.\n");
}

void traiter_requete(char buffer[], int client_sockfd){

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
				/*Si le pseudo est déjà pris, ne le fera jamais puisqu'on considère qu'il n'y a qu'un pseudo unique*/
				already_exist(msg_send);
				send_msg(msg_send, client_sockfd);
				free((*msg_send).msg_content);
				break;
			}
			im_on(msg_send);
			send_msg(msg_send, client_sockfd);
			free((*msg_send).msg_content);
			break;

			case 402: //Pour savoir si untel est en ligne, on renvoi son IP si oui.
			if(exist_user_mysql((*msg_rcv).msg_content)){
				send_ip(msg_rcv, msg_send);
				send_msg(msg_send, client_sockfd);
				free((*msg_send).msg_content);
				break;
			}
			no_exist(msg_send);
			send_msg(msg_send, client_sockfd);
			free((*msg_send).msg_content);
			break;

			case 404: //Cas ou qqun se déco
			if(del_user_mysql((*msg_rcv).msg_content) < 0){
				printf("\e[1;33m[SERVER]\e[0m Un client a essayé de se déconnecter mais il n'est pas enregistré dans la base de données.\n");
			}
			break;

			default:
			break;
		}
	}

	free(msg_send);
	free(msg_rcv);
}
