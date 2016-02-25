#include "clientLib.h"

struct hostent * ask_server_adress(int *port){

	struct hostent *hostinfo;
	char *posPort = NULL;
	char hostname[256]; //Changer 256, et surtout dans les fgets car cela peut poser des problèmes de sécurité
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

int client(int *maxfds, fd_set *readfds, int *num_clients, client_data *fd_array){
	int sock_host;
	struct hostent *hostinfo;
	struct sockaddr_in address;
	// char alias[MSG_SIZE];   a faire
	int port = -1;

	printf("\n*** Enter server's address : ***\n");

	hostinfo = ask_server_adress(&port);

	printf("\n*** Client program starting (enter \"quit\" to stop): ***\n");

	sock_host = socket(AF_INET, SOCK_STREAM, 0);

	address.sin_addr = *(struct in_addr *)*hostinfo -> h_addr_list;
	address.sin_family = AF_INET;
	address.sin_port = htons(port);

	/* Connection au serveur */
	if(connect(sock_host, (struct sockaddr *)&address, sizeof(address)) < 0) { perror("connect"); exit(EXIT_FAILURE); } //gérer autrement car il ne faut pas quitter si on arrive pas a se co

	login_client(&sock_host, fd_array, num_clients, readfds);
	write(sock_host, General_Name, sizeof(General_Name));
	opt_desc(&sock_host, maxfds, readfds); //&*maxfds revient a maxfds

	return 0;
}//main
