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

void session_confirmed(message *segment){

	/*Fonction qui permet d'envoyer le message de session-confirmed*/
    (*segment).code = 202;

	(*segment).msg_content = (char *) malloc(WRITE_SIZE);
	sprintf((*segment).msg_content, "FROM:%s", General_Name);
}

void session_denied(message *segment, int type) {

    /*Fonction qui permet d'envoyer le message de session-denied*/

    (*segment).msg_content = (char *) malloc(WRITE_SIZE);

    if (type == 0) {
        (*segment).code = 300;
        sprintf((*segment).msg_content, "Connection denied : too many clients online);
    } else if (type == 1) {
        (*segment).code = 301; //user already connected
        sprintf((*segment).msg_content, "Connection denied : user already connected);
    }

}

void normal_msg(message *segment, char * data){

	/*Fonction qui permet d'envoyer un message*/

	(*segment).code = 100;
	(*segment).msg_content = (char *) malloc(WRITE_SIZE);

	sprintf((*segment).msg_content, "%s", data);
}

void session_end(message *segment) {

    /*Fonction pour envoyer fin de connection*/

    (*segment).code = 302;

    (*segment).msg_content = (char *) malloc(WRITE_SIZE);
    sprintf((*segment).msg_content, "End of connection of %s", General_Name);
}
