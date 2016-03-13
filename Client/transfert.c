#include "transfert.h"

void init_transfer(int client_sockfd) {
    int fd = 0;
    char filename[256];
    int taille;
    struct stat statut;

    printf("Enter the filename :\n");
    fgets(filename, 256, stdin);
    char *positionEntree = NULL;
    positionEntree = strchr(filename, '\n'); // On recherche l'"Entrée"
    *positionEntree = '\0';
    // mieux gérer les erreurs : if (positionEntree != NULL etc....

    if ((fd = open(filename, O_RDONLY)) < 0) {
        printf("Opening file failed\n");
        return;
    }
    close(fd);

    stat(filename, &statut);
    taille = (int) statut.st_size;

    message *msg = (message *) malloc(sizeof(message));
    transfer_initiate(msg, filename, taille);
    send_msg(msg, &client_sockfd);
    free((*msg).msg_content);
    free(msg);
}

int ask_transfer(message *msg, char *filename) {
    char user[16];
    int taille;
    char res[10];
    int file_fd;

    sscanf(msg->msg_content, "%[^'/']/%[^'/']/%d", user, filename, &taille);
    printf("%s wants to transfer a file : %s (%d bytes). Do you want to accept ? Type y or n.", user, filename, taille);
    fgets(res, 4, stdin);

    if ((strcmp(res, "yes\n")==0) || (strcmp(res, "y\n")==0)) { // if accept
        if ((file_fd = open(filename, O_WRONLY)) > 0) { // if file exists
            close(file_fd);
            printf("%s already exists, if you continue it will be erased. Do you want to continue ? Type y or n.", filename);
            fgets(res, 4, stdin);
            if ((strcmp(res, "yes\n")==0) || (strcmp(res, "y\n")==0)) { // if continue
                if ((file_fd = open(filename, O_TRUNC | O_WRONLY)) < 0)
                    printf("An error has occurred\n"); // fd < 0
            } else { // if not continue
                file_fd = -1;
            }
        } else { // if file doesn't exist
            if ((file_fd = open(filename, O_CREAT | O_WRONLY)) < 0)
                printf("An error has occurred\n"); // fd < 0
        }

    } else { //if not accept
        file_fd = -1;
    }
    return file_fd; //if fd < 0, send transfer_refused
}

void prepare_transfer(message *msg, int client_sockfd) {
    int file_fd = 0;
    char tab[256+2];
    char filename[256];
    sscanf(msg->msg_content, "%[^'/']/%s", filename, filename); // on garde que le %s dans filename
    if ((file_fd = open(filename, O_RDONLY)) < 0) {
        printf("Opening file failed\n");
        return; // faire un transfer_aborted
    }
    sprintf(tab, "%d/%d/%s", file_fd, client_sockfd, filename);

    if(pthread_create(&pid_transfer, NULL, file_transfer, (void *)tab) != 0){
        perror("Probleme avec pthread_create");
        exit(EXIT_FAILURE); // A voir ce qu'on fait ?!;;/:?;:!;:???
    }
}

void *file_transfer(void *arg) {
    char *tab = (char *) arg;
    int file_fd = 0;
    int client_sockfd = 0;
    char filename[256];
    char buffer[WRITE_SIZE];
    message *msg = (message *) malloc(sizeof(message));

    sscanf(tab, "%d/%d/%s", &file_fd, &client_sockfd, filename);

	while ((read(file_fd, buffer, WRITE_SIZE) > 0)) {
        transfer_msg(msg, buffer);
        send_msg(msg, &client_sockfd);
	}

    transfer_end(msg, filename);
    send_msg(msg, &client_sockfd);

    close(file_fd);
    free((*msg).msg_content);
    pthread_exit(NULL);
}
