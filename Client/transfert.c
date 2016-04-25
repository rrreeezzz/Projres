#include "transfert.h"

void init_transfer(char *filepath, int client_fd, fd_set *readfds, client_data *fd_array, int *num_clients) {

  int fd = 0;
  //char filename[256];
  int taille;
  struct stat statut;

  if ((fd = access(filepath, R_OK)) < 0) {
    printf(BLUE"[PROGRAM] Testing file for transfer failed : either file doesn't exist or you don't have permission to read."RESET"\n");
    return;
  }

  if(stat(filepath, &statut) < 0) {perror("Error with file opened"); return;}
  taille = (int) statut.st_size;

  message *msg = (message *) malloc(sizeof(message));
  transfer_initiate(msg, filepath, taille);
  send_msg(msg, &client_fd,readfds,fd_array,num_clients);
  free((*msg).msg_content);
  free(msg);
}


void parser_transfer(int mod, char *msg, char *user, char *filename, int *taille) {
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
  if (mod == 1) {
    sprintf(user, "%s", tab[0]);
    sprintf(filename, "%s", tab[1]);
    *taille = atoi(tab[2]);
  } else if (mod == 2) {
    sprintf(filename, "%s", tab[1]);
  }
}


int ask_transfer(message *msg, char *filename) {
  char user[16];
  int taille;
  char res[10];
  int file_fd;

  parser_transfer(1, msg->msg_content, user, filename, &taille);

  printf("%s wants to transfer a file : %s (%d bytes). Do you want to accept ? Type y or n.\n", user, filename, taille);
  fgets(res, 4, stdin);

  if ((strcmp(res, "yes\n")==0) || (strcmp(res, "y\n")==0)) { // if accept
    if ((file_fd = open(filename, O_WRONLY)) > 0) { // if file exists
      close(file_fd);
      printf("%s already exists, if you continue it will be erased. Do you want to continue ? Type y or n.\n", filename);
      fgets(res, 4, stdin);
      if ((strcmp(res, "yes\n")==0) || (strcmp(res, "y\n")==0)) { // if continue
        if ((file_fd = open(filename, O_CREAT | O_TRUNC | O_WRONLY, 0755)) < 0)
        printf("An error has occurred\n"); // fd < 0
        printf("Transfer in progress \n\n");
      } else { // if not continue
        file_fd = -1;
      }
    } else { // if file doesn't exist
    if ((file_fd = open(filename, O_CREAT | O_WRONLY, 0755)) < 0)
    printf("An error has occurred\n"); // fd < 0
    printf("Transfer in progress \n\n");
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

  parser_transfer(2, msg->msg_content, NULL, filename, NULL);
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
    exit(EXIT_FAILURE); // A voir ce qu'on fait ?!
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
  int n=0;

  message *msg = (message *) malloc(sizeof(message));

  //On recupere les donnees de la structure
  file_fd = data->file_fd;
  client_sockfd = data->client_sockfd;
  strcpy(filename,data->filename);
  strcat(filename, "\0");
  readfds = data->readfds;
  fd_array = data->fd_array;
  num_clients = data->num_clients;

  while ((n=read(file_fd, buffer, WRITE_SIZE))> 0) {
   	transfer_msg(msg, buffer, n);
   	send_msg(msg, &client_sockfd,readfds,fd_array,num_clients);
   	memset(buffer, '\0', WRITE_SIZE);
   	//usleep(100);
  }

  usleep(100); //Au cazouuu
  transfer_end(msg, filename);
  send_msg(msg,&client_sockfd,readfds,fd_array,num_clients);

  printf(BLUE"Transfer of file "RED"%s "BLUE"succeed"RESET"\n", filename); // rajouter le nom à qui on a envoyé
  close(file_fd);
  free((*msg).msg_content);
  pthread_exit(NULL);
}


void prepare_vocal(int client_sockfd, fd_set *readfds, client_data *fd_array, int *num_clients) {
  int file_fd = 0;
  paramThread data;

  if ((file_fd = open("vocal.wav", O_RDONLY)) < 0) {
    printf("Using vocal message failed\n");
    return;
  }

  //On remplit la structure
  data.file_fd = file_fd;
  data.client_sockfd = client_sockfd;
  data.readfds = readfds;
  data.fd_array = fd_array;
  data.num_clients = num_clients;

  if(pthread_create(&pid_transfer, NULL, vocal_transfer, (void *) &data) != 0){
    perror("Probleme avec pthread_create");
    exit(EXIT_FAILURE); // A voir ce qu'on fait ?!
  }
}

void * vocal_transfer(void *arg) {
  //On récupère la structure
  paramThread * data = (paramThread*) arg;
  int file_fd;
  int client_sockfd;
  char buffer[WRITE_SIZE];
  fd_set *readfds;
  client_data *fd_array;
  int *num_clients;
  int n=0;

  message *msg = (message *) malloc(sizeof(message));

  //On recupere les donnees de la structure
  file_fd = data->file_fd;
  client_sockfd = data->client_sockfd;
  readfds = data->readfds;
  fd_array = data->fd_array;
  num_clients = data->num_clients;

  vocal_begin(msg);
  send_msg(msg, &client_sockfd,readfds,fd_array,num_clients);
  usleep(500); //Au cazou


  while ((n=read(file_fd, buffer, WRITE_SIZE))> 0) {
    vocal_msg(msg, buffer, n);
    send_msg(msg, &client_sockfd,readfds,fd_array,num_clients);
    memset(buffer, '\0', WRITE_SIZE);
    //usleep(100);
  }

  usleep(100); //Au cazouuu
  vocal_end(msg);
  send_msg(msg,&client_sockfd,readfds,fd_array,num_clients);

  close(file_fd);
  free((*msg).msg_content);
  pthread_exit(NULL);
}
