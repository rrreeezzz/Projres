#include "message.h"

void im_on(message *segment) {

	/*Fonction qui permet au serveur de répondre au 400, pour savoir s'il est ON, et si celui-ci
	a bien été enregistré. */

	(*segment).code = 401;
	(*segment).msg_content = (char *) malloc(WRITE_SIZE);
	sprintf((*segment).msg_content, "ON");
}

void already_exist(message *segment){

	/*Fonction qui permet au serveur de répondre au 400, pour lui dire qu'il y a déjà qqun
	avec ce pseudo. */

	(*segment).code = 410;
	(*segment).msg_content = (char *) malloc(WRITE_SIZE);
	sprintf((*segment).msg_content, "CHANGE_NICK");
}

void send_ip(message *rcv, message *segment){

	/*Fonction qui permet au serveur de répondre au 402, lui donne l'ip de la personne recherchée.*/

	char name[MAX_SIZE_PARAMETER];
	strcpy(name, (*rcv).msg_content);

	(*segment).code = 403;
	(*segment).msg_content = (char *) malloc(WRITE_SIZE);

	ip_user_mysql(name, (*segment).msg_content);
}

void no_exist(message *segment){

	/*Fonction qui permet au serveur de répondre au 402, lui dit que la personne recherchée n'est pas en ligne.*/

	(*segment).code = 411;
	(*segment).msg_content = (char *) malloc(WRITE_SIZE);
	sprintf((*segment).msg_content, "NOT_ONLINE");
}
