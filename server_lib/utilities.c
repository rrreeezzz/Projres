#include "utilities.h"

int get_Config(char * conf){

  /*Fonction qui permet d'aller chercher les arguments des options dans le fichier conf.txt.
  Celle-ci prend en argument l'option, et renvois l'argument dans le même buffer.*/

  char line[MAX_SIZE_PARAMETER];
  FILE * f;

  sprintf(line, "grep '%s' conf.txt | cut -d':' -f2",conf);
  f = popen(line, "r");
  if(f == NULL){
    perror("Failed to fetch argument.");
    return -1;
  }

  fgets(conf, MAX_SIZE_PARAMETER, f);
  pclose(f);

  return 0;
}

int protocol_parser(char msg[], message *msg_rcv) {

	/*Fonction qui se charge de séparer les différents champs de la trame reçu*/

	char code[3], length[MSG_SIZE], send_time[MSG_SIZE], data[MSG_SIZE];
	(*msg_rcv).msg_content = malloc(MSG_SIZE);

	if(sscanf(msg, "%[^/]/%[^/]/%[^/]/%[^/]/", code, length, send_time, data) == 4){
		(*msg_rcv).code = atoi(code);
		(*msg_rcv).length = atoi(length);
		(*msg_rcv).temps = (time_t) atoi(send_time);
		strcpy((*msg_rcv).msg_content, data);

		return 0;
	}

	return -1;
}

void send_msg(message *segment, int client_sockfd) {

	/*Fonction qui concatène et envois les trames*/

	(*segment).temps = time(NULL);
	(*segment).length = strlen((*segment).msg_content);
	char msg[MSG_SIZE];

	sprintf(msg, "%d/%d/%d/%s", (*segment).code, (*segment).length, (int) (*segment).temps, (*segment).msg_content);
	//printf("msg envoyé : %s, client : %d\n", msg, client_sockfd); //pour debug

	if (write(client_sockfd, msg, MSG_SIZE) <= 0){
		perror("write error.");
	}
}
