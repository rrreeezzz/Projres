#include "transfert.h"

void init_transfer(int client_sockfd) {
    int fd = 0;
    char filename[256];
    int taille;
    struct stat statut;

    printf("Enter the filename :\n");
    fgets(filename, 256, stdin);
    if ((fd = open(filename, O_RDONLY)) < 0) {
        printf("Opening file failed\n");
        return;
    }
    close(fd);

    stat(filename, &statut);
    taille = (int) statut.st_size;

    message *msg = (message *) malloc(sizeof(message));
    transfer_initiate(msg, filename, taille);
    send_msg(msg_send,client_sockfd);
    free((*msg).msg_content);
    free(msg);
}

int ask_transfer(char *msg, char *filename) {
    char user[16];
    int taille;
    char res[10];
    int file_fd;

    sscanf(msg, "%[^'/']/%[^'/']/%d", user, filename, taille);
    printf("%s wants to transfer a file : %s (%d bytes). Do you want to accept ? Type y or n.");
    fgets(res, 4, stdin);

    if ((strcmp(msg, "yes\n")==0) || (strcmp(msg, "y\n")==0)) { // if accept
        if ((file_fd = open(filename, O_WRONLY)) > 0) { // if file exists
            close(file_fd);
            printf("%s already exists, if you continue it will be erased. Do you want to continue ? Type y or n.");
            fgets(res, 4, stdin);
            if ((strcmp(msg, "yes\n")==0) || (strcmp(msg, "y\n")==0)) { // if continue
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

void prepare_transfer(char *msg, int client_sockfd) {
    int file_fd;
    char filename[256] = sscanf(msg, "%[^'/']/%s", filename, filename); // on garde que le %s dans filename
    if ((file_fd = open(filename, O_RDONLY)) < 0) {
        printf("Opening file failed\n");
        return; // faire un transfer_aborted
    }
    tab[0] = file_fd;
    tab[1] = client_sockfd;

    if(pthread_create(&pid_transfer, NULL, file_transfer, (void *)tab) != 0){
        perror("Probleme avec pthread_create");
        exit(EXIT_FAILURE); // A voir ce qu'on fait ?!;;/:?;:!;:???
    }
}

void *file_transfer(void *arg) {
    int *tab = (int *) arg;  //tab[0] = file_fd   tab[1] = client_sockfd
    int file_fd = tab[0];
    int client_sockfd = tab[1];
    char buffer[WRITE_SIZE];
    message *msg = (message *) malloc(sizeof(message));

	while ((nchar = read(file_fd, buffer, WRITE_SIZE))) {
        transfer_msg(msg, buffer);
        send_msg(msg, client_sockfd);
	}

    transfer_end(msg, filename);
    send_msg(msg, client_sockfd);

    close(file_fd);
    free((*msg).msg_content);
    pthread_exit(NULL);
}
