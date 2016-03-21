#include "transfert.h"

void init_transfer(int client_sockfd, fd_set *readfds, client_data *fd_array, int *num_clients) {
    int fd = 0;
    char filename[256];
    int taille;
    struct stat statut;

    printf("Enter the filename :\n");
    fgets(filename, 256, stdin);
    char *positionEntree = NULL;
    if((positionEntree = strchr(filename, '\n')) == NULL) { // On recherche l'"Entree"
      *positionEntree = '\0';
    }// mieux gerer les erreurs : if (positionEntree != NULL etc....

    if ((fd = open(filename, O_RDONLY)) < 0) {
        printf("Opening file failed : wrong filename\n");
        return;
    }
    close(fd);

    if(stat(filename, &statut) < 0) {perror("Error with file opened"); exit(EXIT_FAILURE);}
    taille = (int) statut.st_size;

    message *msg = (message *) malloc(sizeof(message));
    transfer_initiate(msg, filename, taille);
    send_msg(msg, &client_sockfd,readfds,fd_array,num_clients);
    free((*msg).msg_content);
    free(msg);
}


void parser_transfer(char *msg, char *user, char *filename, int *taille) {
  char * pch;
  char tab[3][strlen(msg)];
  int i = 0;
  pch = strtok (msg," |");
  while (pch != NULL)
  {
    sprintf(tab[i], "%s", pch);
    pch = strtok (NULL, " |");
    i++;
  }
sprintf(tab[0], "%s", tab[0]);
sprintf(tab[1], "%s", tab[1]);
 /* user = tab[0];*///memcpy(user, tab[0], strlen(tab[0]));
 /* filename = tab[1];*///memcpy(filename, tab[1], strlen(tab[1]));
sprintf(user, "%s", tab[0]);
sprintf(filename, "%s", tab[1]);
  *taille = atoi(tab[2]);
}


int ask_transfer(message *msg, char *filename) {
    char user[16];
    int taille;
    char res[10];
    int file_fd;

   parser_transfer(msg->msg_content, user, filename, &taille);

    printf("%s wants to transfer a file : %s (%d bytes). Do you want to accept ? Type y or n.", user, filename, taille);
    fgets(res, 4, stdin);

    if ((strcmp(res, "yes\n")==0) || (strcmp(res, "y\n")==0)) { // if accept
        if ((file_fd = open(filename, O_WRONLY)) > 0) { // if file exists
            close(file_fd);
            printf("%s already exists, if you continue it will be erased. Do you want to continue ? Type y or n.", filename);
            fgets(res, 4, stdin);
            if ((strcmp(res, "yes\n")==0) || (strcmp(res, "y\n")==0)) { // if continue
                if ((file_fd = open(filename, O_TRUNC | 0755)) < 0)
                    printf("An error has occurred\n"); // fd < 0
            } else { // if not continue
                file_fd = -1;
            }
        } else { // if file doesn't exist
            if ((file_fd = open(filename, O_CREAT | 0755)) < 0)
                printf("An error has occurred\n"); // fd < 0
        }

    } else { //if not accept
        file_fd = -1;
    }
    return file_fd; //if fd < 0, send transfer_refused
}

void prepare_transfer(message *msg, int client_sockfd, fd_set *readfds, client_data *fd_array, int *num_clients) {
    int file_fd = 0;
    paramThread data;

    char filename[256];
    sscanf(msg->msg_content, "%[^'/']/%s", filename, filename); // on garde que le %s dans filename
    if ((file_fd = open(filename, O_RDONLY)) < 0) {
        printf("Opening file failed\n");
        return; // faire un transfer_aborted
    }

    //On remplit la structure
    data.file_fd = file_fd;
    data.client_sockfd = client_sockfd;
    strcpy(data.filename,filename);
    data.readfds = readfds;
    data.fd_array = fd_array;
    data.num_clients = num_clients;

    if(pthread_create(&pid_transfer, NULL, file_transfer, (void *) &data) != 0){
        perror("Probleme avec pthread_create");
        exit(EXIT_FAILURE); // A voir ce qu'on fait ?!;;/:?;:!;:???
    }
}

void * file_transfer(void *arg) {
    //On récupère la structure
    paramThread * data = (paramThread*) arg;
    int file_fd;
    int client_sockfd;
    char filename[256];
    char buffer[WRITE_SIZE];
    fd_set *readfds;
    client_data *fd_array;
    int *num_clients;

    message *msg = (message *) malloc(sizeof(message));

    //On recupere les donnees de la structure
    file_fd = data->file_fd;
    client_sockfd = data->client_sockfd;
    strcpy(filename,data->filename);
    readfds = data->readfds;
    fd_array = data->fd_array;
    num_clients = data->num_clients;

	while ((read(file_fd, buffer, WRITE_SIZE) > 0)) {
        transfer_msg(msg, buffer);
        send_msg(msg, &client_sockfd,readfds,fd_array,num_clients);
	}

    transfer_end(msg, filename);
    send_msg(msg,&client_sockfd,readfds,fd_array,num_clients);

    close(file_fd);
    free((*msg).msg_content);
    pthread_exit(NULL);
}
