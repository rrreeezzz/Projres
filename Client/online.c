#include "online.h"

int connect_serv(){

  online = 0;

  int fd_tmp;
  char msg[MSG_SIZE];
  char temp[MSG_SIZE];
  struct ifreq my_ip;
  struct sockaddr_in info_online;
  //time_t tps = time(NULL);

  /*On recup l'adresse ip sur eth0*/
  my_ip.ifr_addr.sa_family = AF_INET;
  /*ADRESSE de eth0 impossible a prendre, a corriger*/
  strncpy(my_ip.ifr_name, "eth0", IFNAMSIZ-1);
  ioctl(fd_tmp, SIOCGIFADDR, &my_ip);

  info_online.sin_family = AF_INET;
  inet_aton("0.0.0.0", (struct in_addr *) &info_online.sin_addr.s_addr); //recup adresse serveur ?
  info_online.sin_port = htons(40000); //port par defaut du serveur

  online = socket(AF_INET, SOCK_STREAM, 0);
  /* Connection au serveur annuaire */
  if(connect(online, (struct sockaddr *)&info_online, sizeof(info_online)) < 0) {
    perror("Erreur de connection, le serveur ne semble pas être en ligne");
    return -1;
  } //gérer autrement car il ne faut pas quitter si on arrive pas a se co
  /*A mettre si pas de serveur hebergé ?*/

  printf("[PROGRAM] : Your ip is %s \n", inet_ntoa(((struct sockaddr_in *) &my_ip.ifr_addr)->sin_addr));
  printf("[PROGRAM] : Server ip is %s \n", inet_ntoa(info_online.sin_addr));

  sprintf(temp, "FROM:%s IP:%s", General_Name, inet_ntoa(((struct sockaddr_in *) &my_ip.ifr_addr)->sin_addr));
  /*BUG TIME(NULL) A CORRIGER*/
  sprintf(msg, "400/11111111/%d/%s/END", strlen(temp), temp);

  if(write(online, msg, MSG_SIZE) <= 0){
    perror("Erreur write serveur annuaire");
    return -1;
  }

  printf("[PROGRAM] : You'r IP has been send.\n");
  close(online);
  close(fd_tmp);
  return 0;
}

int search_serv(char *buf, client_data *fd_array, int *num_clients, fd_set *readfds){

  online = 0;

  char msg[MSG_SIZE];
  char temp[MSG_SIZE];
  char name[MAX_SIZE_USERNAME];
  char *posSpace = NULL;
  struct sockaddr_in info_online;
  //time_t tps = time(NULL);

  if((posSpace = strchr(buf, ' ')) == NULL){
    printf("[PROGRAM] Error command.");
    return -1;
  }

  strcpy(name, posSpace+1);
  name[strlen(name) - 1] = '\0';


  info_online.sin_family = AF_INET;
  inet_aton("0.0.0.0", (struct in_addr *) &info_online.sin_addr.s_addr); //recup adresse serveur ?
  info_online.sin_port = htons(40000); //port par defaut du serveur

  online = socket(AF_INET, SOCK_STREAM, 0);
  /* Connection au serveur annuaire */
  if(connect(online, (struct sockaddr *)&info_online, sizeof(info_online)) < 0) {
    perror("Erreur de connection, le serveur ne semble pas être en ligne");
    return -1;
  } //gérer autrement car il ne faut pas quitter si on arrive pas a se co
  /*A mettre si pas de serveur hebergé ?*/

  printf("[PROGRAM] : Server ip is %s \n", inet_ntoa(info_online.sin_addr));

  sprintf(temp, "%s", name);
  /*BUG TIME(NULL) A CORRIGER*/
  sprintf(msg, "402/11111111/%d/%s/END", strlen(temp), temp);

  if(write(online, msg, MSG_SIZE) <= 0){
    perror("Erreur write serveur annuaire");
    return -1;
  }

  memset(msg, '\0', MSG_SIZE);

  if(read(online, msg, MSG_SIZE) <= 0){
    perror("Erreur write serveur annuaire");
    return -1;
  }

  rechercheProtocol(msg, &online, fd_array, num_clients, readfds);
  close(online);
  return 0;
}
