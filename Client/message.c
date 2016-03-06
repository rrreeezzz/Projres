#include "message.h"

void session_initiate(message *segment){

	/*Fonction qui permet d'envoyer le message de session-initiate*/

	(*segment).code = 200;
	(*segment).msg_content = (char *) malloc(WRITE_SIZE);

	sprintf((*segment).msg_content, "FROM:%s", General_Name);

}

void session_accept(message *segment){

	/*Fonction qui permet d'envoyer le message de session-accept*/

	(*segment).code = 201;
	(*segment).msg_content = (char *) malloc(WRITE_SIZE);

	sprintf((*segment).msg_content, "FROM:%s", General_Name);

}

void normal_msg(message *segment, char * data){

	/*Fonction qui permet d'envoyer le message de session-accept*/

	(*segment).code = 100;
	(*segment).msg_content = (char *) malloc(WRITE_SIZE);

	sprintf((*segment).msg_content, "%s", data);

}
