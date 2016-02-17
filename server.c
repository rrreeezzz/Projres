#include "server.h"

int rechercheCmd(const char *msg) { //les erreurs de cmd seront gérées côté client et la cmd help aussi !!!!!!!
	if (*msg == '/') {
		if (!strncmp(msg+1, "quit", 4)) { return QUIT;
		} else if (!strncmp(msg+1, "msg", 3)) { return MSG;		
		} else if (!strncmp(msg+1, "grp", 3)) { return GRP;
		} else { return ALL1; }
	} else { return ALL2; }
}

void exitClient(int fd, fd_set *readfds, char *fd_array, int *num_clients){
    int i;
    close(fd);
    FD_CLR(fd, readfds); //on enlève le leaver du tableau de clients
    for (i = 0; i < (*num_clients) - 1; i++)
        if (fd_array[i] == fd)
            break;          
    for (; i < (*num_clients) - 1; i++)
        (fd_array[i]) = (fd_array[i + 1]);
    (*num_clients)--;
}

void traiterRequete(int fd, fd_set *readfds, char *fd_array, int *num_clients) {
    char msg[WRITE_SIZE];
    char rep_msg[MSG_SIZE];
    int i, result;
    
    if ((result = read(fd, msg, WRITE_SIZE)) > 0) { /* Une requête en attente sur le descripteur fd */
	  msg[result] = '\0';
     	  printf("Client %d : %s", fd, msg); // a modifier avec le pseudo du mec
	  
      switch(rechercheCmd(msg)) {
		case QUIT:
			printf("--- Client %d left\n", fd);  // a modifier avec le pseudo du mec
			exitClient(fd, readfds, fd_array, num_clients);
			break;
		case MSG:
			//on envoie que au client concerné   rajouter notre structure en param de la fonction
			break;
		case GRP:
			//on envoie au groupe
			break;
		case ALL1: // comande /all
			sprintf(rep_msg, "%d : %s", fd, msg+5); //msg+5 pour enlever le /all
			for(i=0; i<(*num_clients); i++){
                		if (fd_array[i] != fd)  /* on ne renvoie pas à l'emetteur */
                   			if ((result = write(fd_array[i], rep_msg, strlen(rep_msg))) < 0) { perror("write ALL1"); exit(EXIT_FAILURE); } 
            		}
			break;
		case ALL2: //ici pas de /all
			sprintf(rep_msg, "%d : %s", fd, msg);
			for(i=0; i<(*num_clients); i++){
                		if (fd_array[i] != fd)  /* on ne renvoie pas à l'emetteur */
                    			if ((result = write(fd_array[i], rep_msg, strlen(rep_msg))) < 0) { perror("write ALL2"); exit(EXIT_FAILURE); } 
            		}
			break;
	  } //switch
						  
    } else {
		printf("--- End of connection of client %d\n", fd);  // a modifier avec le pseudo du mec
        exitClient(fd, readfds, fd_array, num_clients);
    } //if read
}


int main(int argc, char *argv[]) {
	
   int i=0;
   int num_clients = 0;
   int server_sockfd, client_sockfd;
   struct sockaddr_in server_address, client_address;
   int addresslen = sizeof(struct sockaddr_in);
   int fd;
   char fd_array[MAX_CLIENTS];
   fd_set readfds, testfds;
   int maxfds;
   char msg[WRITE_SIZE];
   char rep_msg[MSG_SIZE];
     
	 
     printf("\n*** Server program starting (enter \"quit\" to stop) ***\n");

     server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
     server_address.sin_family = AF_INET;
     server_address.sin_addr.s_addr = htonl(INADDR_ANY);
     server_address.sin_port = 0;
     if (bind(server_sockfd, (struct sockaddr *)&server_address, addresslen) < 0) { perror("bind"); exit(EXIT_FAILURE); }
     if (getsockname(server_sockfd, (struct sockaddr *)&server_address, (socklen_t *)&addresslen) < 0) { perror("getsockname"); exit(EXIT_FAILURE); }
	 
     printf("\n*** Connections available on port : %d ***\n", ntohs(server_address.sin_port));

     maxfds = server_sockfd;
     if(listen(server_sockfd, 1) < 0) { perror("listen"); exit(EXIT_FAILURE); } // mettre plus que 1 utile ???
     FD_ZERO(&readfds);
     FD_SET(server_sockfd, &readfds);
     FD_SET(0, &readfds);  /* On ajoute le clavier au file descriptor set */

     /*  Attente de clients et de requêtes */
     while (1) {
        testfds = readfds;
        if(select(maxfds+1, &testfds, NULL, NULL, NULL) < 0) { perror("select"); exit(EXIT_FAILURE); }
                    
        /* Il y a une activité, on cherche sur quel descripteur grâce à FD_ISSET */
        for (fd=0; fd<maxfds+1; fd++) {
           if (FD_ISSET(fd, &testfds)) {
              
              if (fd == server_sockfd) { /* Accept des nouvelles connections */
		 if((client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, (socklen_t *)&addresslen )) < 0 ) { perror("accept"); exit(EXIT_FAILURE); }
                                                
                 if (num_clients < MAX_CLIENTS) {
				 /* Ajouter client_sockfd dans la liste des descripteurs en attente */
				 /* Pour cela, on cherche si un descripteur plus petit est libre */
				 /* Optimisation de descripteurs*/
					 int descDup;
					 descDup = dup(client_sockfd);
					 if (client_sockfd < descDup) { 
						 close(descDup); 
						 maxfds = Max(client_sockfd, maxfds); 
						 FD_SET(client_sockfd, &readfds); 
					 } else {
						 close(client_sockfd); 
						 maxfds = Max(descDup, maxfds); 
						 FD_SET(descDup, &readfds); 
					 }
		 
                    fd_array[num_clients]=client_sockfd;					
                    printf("--- Client %d joined\n",client_sockfd);             ///*** a modif avec le pseudo du mec
					sprintf(msg, "[SERVER] : Welcome !\n");
					write(fd_array[num_clients], msg, strlen(msg));
					num_clients++;
                 } else {
					 printf("--- Someone tried to connect, but too many clients online\n");
                     sprintf(msg, "X[SERVER] : Sorry, too many clients online.  Try again later.\n"); // modifier le X pour dire au client de couper
                     write(client_sockfd, msg, strlen(msg));
                     close(client_sockfd);
                 } //if num_clients < MAX_CLIENTS
				 
              } else if (fd == 0) {  /*activité sur le clavier*/                 
                 fgets(msg, WRITE_SIZE, stdin);
                 if (strcmp(msg, "quit\n")==0) {      // A arranger avec plus de tests : si longueur 4 et quit ou des trucs du genre
					printf("--- Server is shutting down\n");
                    sprintf(msg, "X[SERVER] : Server is shutting down.\n");  // modifier le X pour dire au client de couper
                    for (i = 0; i < num_clients ; i++) {
                       write(fd_array[i], msg, strlen(msg));
                       close(fd_array[i]);
                    }
                    close(server_sockfd);
                    exit(0);
                 } else {
                    sprintf(rep_msg, "[SERVER] : %s", msg);
                    for (i=0; i<num_clients ; i++)
                       write(fd_array[i], rep_msg, strlen(rep_msg));
                 }
				 
              } else {  /*activité d'un client*/
                 traiterRequete(fd, &readfds, fd_array, &num_clients);		 
              }//if fd ==
           }//if FD_ISSET
        }//for
     }//while
	exit(EXIT_SUCCESS);
}//main

