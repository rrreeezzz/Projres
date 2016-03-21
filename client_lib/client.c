#include "client.h"

struct hostent * ask_server_adress(int *port){

	struct hostent *hostinfo;
	char *posPort = NULL;
	char hostname[256]; //Changer 256 en macro ?
	char temp[256];

	while(strlen(hostname) == 0 || *port == -1){
		fgets(hostname, 256, stdin);
		if((posPort = strchr(hostname, ' ')) != NULL){
			*port = (int) strtol(posPort, NULL, 10);
			if(*port <= 0 || *port >= 100000)
			*port = -1;
		}else if((posPort = strchr(hostname, ':')) != NULL){
			*port = (int) strtol(posPort+1, NULL, 10);
			if(*port <= 0 || *port >= 100000)
			*port = -1;
		}else if(posPort == NULL || *port == -1){
			printf("\n-----Please enter correct adress-----\n");
			continue;
		}

		strncpy(temp, hostname, ((int) strlen(hostname) - (int) strlen(posPort)));
		temp[((int) strlen(hostname) - (int) strlen(posPort))] = '\0';

		if((hostinfo = gethostbyname(temp)) == NULL)
		memset(hostname, 0, sizeof (hostname));
	}

	return hostinfo;
}


void viderBuffer() {
	int c = 0;
	while (c != '\n' && c != EOF) {
		c = getchar();
	}
}

int client(){
	fd_set testfds, clientfds;
	char msg[MSG_SIZE];
	int fd;
	int result;
	int sockfd;
	int maxfds;
	struct hostent *hostinfo;
	struct sockaddr_in address;
	// char alias[MSG_SIZE];   a faire
	int port = -1;

	printf("\n*** Enter server's address : ***\n");     // A modifier pour qu'on puisse rentrer 127.0.0.1:2500 ou 127.0.0.1 2500, et dans ce cas ça demande pas à nouveau le port

	hostinfo = ask_server_adress(&port);

	printf("\n*** Client program starting (enter \"quit\" to stop): ***\n");

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	address.sin_addr = *(struct in_addr *)*hostinfo -> h_addr_list;
	address.sin_family = AF_INET;
	address.sin_port = htons(port);

	/* Connection au serveur */
	if(connect(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0) { perror("connect"); exit(EXIT_FAILURE); }

	maxfds = sockfd;
	FD_ZERO(&clientfds);
	FD_SET(sockfd, &clientfds);
	FD_SET(0, &clientfds); /* On ajoute le clavier au file descriptor set */

	/*  Attente de messages du serveur */
	while (1) {
		testfds = clientfds;
		if(select(maxfds+1, &testfds, NULL, NULL, NULL) < 0) { perror("select"); exit(EXIT_FAILURE); }

		for(fd=0; fd<maxfds+1; fd++){
			if(FD_ISSET(fd, &testfds)){
				if(fd == sockfd){   /* des données arrivent */
					result = read(sockfd, msg, MSG_SIZE);
					msg[result] = '\0';
					printf("%s", msg);

					if (msg[0] == 'X') {        // Condition pour couper connection à modifier
						close(sockfd);
						exit(EXIT_SUCCESS);
					}
				}
				else if(fd == 0){ /*activité sur le clavier*/       //implémenter fonction rechercheCmd pour tester les différentes cmd et gérer les erreurs
					if (fgets(msg, WRITE_SIZE, stdin) != NULL) {
						if (strcmp(msg, "/quit\n")==0) {
							write(sockfd, msg, strlen(msg));
							close(sockfd);
							exit(EXIT_SUCCESS); //end program
						} else {
							write(sockfd, msg, strlen(msg));
						}
					} else { viderBuffer(); }
				}  // if fd ==
			} // if FD_ISSET
		} // for
	} // while 1
	exit(EXIT_SUCCESS);
}//main
