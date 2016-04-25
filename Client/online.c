#include "online.h"

int connect_serv(){

  online = 0;

  char msg[MSG_SIZE];
  char temp[MSG_SIZE];
  struct sockaddr_in info_online;
  time_t tps = time(NULL);

  struct ifaddrs *my_ip;
	struct sockaddr_in *read_ip;

	getifaddrs(&my_ip);

	struct ifaddrs *my_ip_curs = my_ip;

	while(my_ip_curs->ifa_next != NULL){

		read_ip = (struct sockaddr_in *) my_ip_curs->ifa_addr;
		if(read_ip->sin_family == AF_INET && strcmp(my_ip_curs->ifa_name, "eth0") == 0){
      break;
    }
		my_ip_curs = my_ip_curs->ifa_next;
	}

	freeifaddrs(my_ip);

  info_online.sin_family = AF_INET;
  inet_aton("0.0.0.0", (struct in_addr *) &info_online.sin_addr.s_addr); //recup adresse serveur ?
  info_online.sin_port = htons(40000); //port par defaut du serveur

  online = socket(AF_INET, SOCK_STREAM, 0);
  /* Connection au serveur annuaire */
  if(connect(online, (struct sockaddr *)&info_online, sizeof(info_online)) < 0) {
    perror(BLUE"Erreur de connection, le serveur ne semble pas être en ligne"RESET);
    return -1;
  }

  printf(BLUE"[PROGRAM] : Your ip is "RED"%s "RESET"\n", inet_ntoa(read_ip->sin_addr));
  printf(BLUE"[PROGRAM] : Server ip is "RED"%s "RESET"\n", inet_ntoa(info_online.sin_addr));

  sprintf(temp, "FROM:%s IP:%s:%d", General_Name, inet_ntoa(read_ip->sin_addr), General_Port);
  sprintf(msg, "400/%d/%zd/%s/END", (int) tps, strlen(temp), temp);

  if(write(online, msg, MSG_SIZE) <= 0){
    perror("Erreur write serveur annuaire");
    return -1;
  }

  printf(BLUE"[PROGRAM] : Your IP has been send."RESET"\n\n");
  close(online);
  return 0;
}

int search_serv(char *buf, client_data *fd_array, int *num_clients, fd_set *readfds, waitList *waitlist){

  online = 0;

  char msg[MSG_SIZE];
  char temp[MSG_SIZE];
  char name[MAX_SIZE_USERNAME];
  char *posSpace = NULL;
  struct sockaddr_in info_online;
  time_t tps = time(NULL);

  if((posSpace = strchr(buf, ' ')) == NULL){
    printf(BLUE"[PROGRAM] Error command."RESET"\n");
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
    perror(BLUE"Erreur de connection, le serveur ne semble pas être en ligne"RESET);
    return -1;
  }

  printf(BLUE"[PROGRAM] : Server ip is "RED"%s "RESET"\n", inet_ntoa(info_online.sin_addr));

  sprintf(temp, "%s", name);
  sprintf(msg, "402/%d/%zd/%s/END", (int) tps, strlen(temp), temp);

  if(write(online, msg, MSG_SIZE) <= 0){
    perror("Erreur write serveur annuaire");
    return -1;
  }

  memset(msg, '\0', MSG_SIZE);

  if(read(online, msg, MSG_SIZE) <= 0){
    perror("Erreur write serveur annuaire");
    return -1;
  }

  rechercheProtocol(msg, &online, fd_array, num_clients, readfds, waitlist);
  close(online);
  return 0;
}

int erase_serv(){

  online = 0;

  char msg[MSG_SIZE];
  char temp[MSG_SIZE];
  struct sockaddr_in info_online;
  time_t tps = time(NULL);

  info_online.sin_family = AF_INET;
  inet_aton("0.0.0.0", (struct in_addr *) &info_online.sin_addr.s_addr); //recup adresse serveur ?
  info_online.sin_port = htons(40000); //port par defaut du serveur

  online = socket(AF_INET, SOCK_STREAM, 0);
  /* Connection au serveur annuaire */
  if(connect(online, (struct sockaddr *)&info_online, sizeof(info_online)) < 0) {
    perror("Erreur de connection, le serveur ne semble pas être en ligne");
    return -1;
  }

  sprintf(temp, "FROM:%s", General_Name);
  sprintf(msg, "404/%d/%zd/%s/END", (int) tps, strlen(temp), temp);

  if(write(online, msg, MSG_SIZE) <= 0){
    perror("Erreur write serveur annuaire");
    return -1;
  }

  printf(BLUE"[PROGRAM] : You have been erased from the database."RESET"\n");
  close(online);
  return 0;
}
