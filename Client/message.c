#include "message.h"

void session_initiate(message *segment) {

	/*Fonction qui permet d'envoyer le message de session-initiate*/
	(*segment).code = 200;
	(*segment).msg_content = (char *) malloc(WRITE_SIZE);
	sprintf((*segment).msg_content, "%s", General_Name);
	(*segment).length = strlen((*segment).msg_content);
}

void session_accept(message *segment) {

	/*Fonction qui permet d'envoyer le message de session-accept*/
	(*segment).code = 201;
	(*segment).msg_content = (char *) malloc(WRITE_SIZE);
	sprintf((*segment).msg_content, "%s", General_Name);
	(*segment).length = strlen((*segment).msg_content);
}

void session_confirmed(message *segment) {

	/*Fonction qui permet d'envoyer le message de session-confirmed*/
	(*segment).code = 202;
	(*segment).msg_content = (char *) malloc(WRITE_SIZE);
	sprintf((*segment).msg_content, "%s", General_Name);
	(*segment).length = strlen((*segment).msg_content);
}

void session_denied(message *segment, int type) {

	/*Fonction qui permet d'envoyer le message de session-denied*/
	(*segment).msg_content = (char *) malloc(WRITE_SIZE);
	if (type == 0) {
		(*segment).code = 300; //too many clients online
		(*segment).length = 0;
	} else if (type == 1) {
		(*segment).code = 301; //user already connected
    sprintf((*segment).msg_content, "%s", General_Name);
		(*segment).length = strlen((*segment).msg_content);
	} else if (type == 2) {
		(*segment).code = 302; //connection refused by user
		sprintf((*segment).msg_content, "PROGRAM");
		(*segment).length = 0;
	}

}

void normal_msg(message *segment, char *data) {

	/*Fonction qui permet d'envoyer un message*/
	(*segment).code = 100;
	(*segment).msg_content = (char *) malloc(WRITE_SIZE);
	sprintf((*segment).msg_content, "%s", data);
}

void session_end(message *segment) {

	/*Fonction pour envoyer fin de connection*/
	(*segment).code = 303;
	(*segment).msg_content = (char *) malloc(WRITE_SIZE);
	sprintf((*segment).msg_content, "%s", General_Name);
	(*segment).length = strlen((*segment).msg_content);
}

void session_aborted(message *segment) {

	/*Fonction pour envoyer connection annulée*/
	(*segment).code = 304;
	(*segment).msg_content = (char *) malloc(WRITE_SIZE);
	sprintf((*segment).msg_content, "%s", General_Name);
}

void transfer_initiate(message *segment, char *filename, int taille) {

	/*Fonction qui permet d'envoyer le message de transfer-initiate*/
	(*segment).code = 203;
	(*segment).msg_content = (char *) malloc(WRITE_SIZE);
	sprintf((*segment).msg_content, "%s|%s|%d", General_Name, filename, taille);
	(*segment).length = strlen((*segment).msg_content);
}

void transfer_accept(message *segment, char *filename) {

	/*Fonction qui permet d'envoyer le message de transfer-accept*/
	(*segment).code = 204;
	(*segment).msg_content = (char *) malloc(WRITE_SIZE);
	sprintf((*segment).msg_content, "%s|%s", General_Name, filename);
	(*segment).length = strlen((*segment).msg_content);
}

void transfer_begin(message *segment, char *filename) {

	/*Fonction qui permet d'envoyer début de transfert*/
	(*segment).code = 205;
	(*segment).msg_content = (char *) malloc(WRITE_SIZE);
	sprintf((*segment).msg_content, "%s|%s", General_Name, filename);
	(*segment).length = strlen((*segment).msg_content);
}

void transfer_refused(message *segment) {

	/*Fonction qui permet d'envoyer le message de transfer-refused*/
	(*segment).code = 305;
	(*segment).msg_content = (char *) malloc(WRITE_SIZE);
	sprintf((*segment).msg_content, "%s", General_Name);
	(*segment).length = strlen((*segment).msg_content);
}

void transfer_aborted(message *segment) {

	/*Fonction qui permet d'envoyer le message de transfer-refused*/
	(*segment).code = 306;
	(*segment).msg_content = (char *) malloc(WRITE_SIZE);
	sprintf((*segment).msg_content, "%s", General_Name);
	(*segment).length = strlen((*segment).msg_content);
}

void transfer_msg(message *segment, char *data, int n) {

	/*Fonction qui permet d'envoyer un morceau du fichier*/
	(*segment).code = 102;
	(*segment).msg_content = (char *) malloc(WRITE_SIZE);
	memcpy((*segment).msg_content, data, n);
	(*segment).length = n;
}

void transfer_end(message *segment, char *filename) {

	/*Fonction pour envoyer fin de transfert*/
	(*segment).code = 308;
	(*segment).msg_content = (char *) malloc(WRITE_SIZE);
	sprintf((*segment).msg_content, "%s", filename);
	(*segment).length = strlen((*segment).msg_content);
}


void vocal_begin(message *segment) {

	/*Fonction qui permet d'envoyer le message de début du transfert du vocal*/
	(*segment).code = 500;
	(*segment).msg_content = (char *) malloc(WRITE_SIZE);
	sprintf((*segment).msg_content, "%s", General_Name);
	(*segment).length = strlen((*segment).msg_content);
}

void vocal_ok(message *segment) {
	(*segment).code = 501;
	(*segment).msg_content = (char *) malloc(WRITE_SIZE);
	sprintf((*segment).msg_content, "%s", General_Name);
	(*segment).length = strlen((*segment).msg_content);
}

void vocal_nok(message *segment) {
	(*segment).code = 502;
	(*segment).msg_content = (char *) malloc(WRITE_SIZE);
	sprintf((*segment).msg_content, "%s", General_Name);
	(*segment).length = n;
}

void vocal_msg(message *segment, char *data, int n) {

	/*Fonction qui permet d'envoyer un morceau du fichier du vocal*/
	(*segment).code = 503;
	(*segment).msg_content = (char *) malloc(WRITE_SIZE);
	memcpy((*segment).msg_content, data, n);
	(*segment).length = strlen((*segment).msg_content);
}

void vocal_end(message *segment) {

	/*Fonction pour envoyer fin de transfert du vocal*/
	(*segment).code = 504;
	(*segment).msg_content = (char *) malloc(WRITE_SIZE);
	sprintf((*segment).msg_content, "%s", General_Name);
	(*segment).length = strlen((*segment).msg_content);
}

void ping(message *segment) {
	/*Fonction qui permet d'envoyer un message*/
	(*segment).code = 600;
	(*segment).msg_content = (char *) malloc(WRITE_SIZE);
	sprintf((*segment).msg_content, "%s", General_Name);
}

void pong(message *segment) {
	/*Fonction qui permet d'envoyer un message*/
	(*segment).code = 601;
	(*segment).msg_content = (char *) malloc(WRITE_SIZE);
	sprintf((*segment).msg_content, "%s", General_Name);
}
