#include "clientLib.h"

struct hostent * ask_server_adress(int *port){

	struct hostent *hostinfo;
	char *posPort = NULL;
	char hostname[256]; //Changer 256 en macros ?
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

int client(int *maxfds, fd_set *readfds){
	int sockfd;
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

	write(sockfd, General_Name, strlen(General_Name));

	opt_desc(&sockfd, &*maxfds, &*readfds);

	return 0;
}//main
