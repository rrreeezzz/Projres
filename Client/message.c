#include "message.h"

void session_initiate(message *segment){

	/*Fonction qui permet d'envoyer le message de session-initiate*/

	(*segment).code = 200;
	(*segment).msg_content = (char *) malloc(WRITE_SIZE);

	sprintf((*segment).msg_content, "FROM:%s", General_Name);

}

void session_accept(message *segment, int type){

	/*Fonction qui permet d'envoyer le message de session-accept*/
    if (type == 1)
        (*segment).code = 201;
    else if (type == 2)
        (*segment).code = 202;

	(*segment).msg_content = (char *) malloc(WRITE_SIZE);
	sprintf((*segment).msg_content, "FROM:%s", General_Name);

}

void session_denied(message *segment, int type) {

    /*Fonction qui permet d'envoyer le message de session-denied*/

    if (type == 0)
        (*segment).code = 300; //too many clients online
    else if (type == 1)
        (*segment).code = 301; //user already connected

    (*segment).msg_content = (char *) malloc(WRITE_SIZE);
	sprintf((*segment).msg_content, "FROM:%s", General_Name);

}

void normal_msg(message *segment, char * data){

	/*Fonction qui permet d'envoyer un message*/

	(*segment).code = 100;
	(*segment).msg_content = (char *) malloc(WRITE_SIZE);

	sprintf((*segment).msg_content, "%s", data);

}
