/*
* Messages possibles
*/
//message normal
void normal_msg(message *segment, char *data) {
	/*Fonction qui permet d'envoyer un message*/
	(*segment).code = 100;
	(*segment).msg_content = (char *) malloc(WRITE_SIZE);
	sprintf((*segment).msg_content, "%s", data);
}
//Demande de connection
void session_initiate(message *segment) {
	/*Fonction qui permet d'envoyer le message de session-initiate*/
	(*segment).code = 200;
	(*segment).msg_content = (char *) malloc(WRITE_SIZE);
	sprintf((*segment).msg_content, "%s", "USERINTERFACE");
	(*segment).length = strlen((*segment).msg_content);
}
//Confirmation de connection
void session_confirmed(message *segment) {
	/*Fonction qui permet d'envoyer le message de session-confirmed*/
	(*segment).code = 202;
	(*segment).msg_content = (char *) malloc(WRITE_SIZE);
	sprintf((*segment).msg_content, "%s", "USERINTERFACE");
	(*segment).length = strlen((*segment).msg_content);
}
//Deconnection
void session_end(message *segment) {
	/*Fonction pour envoyer fin de connection*/
	(*segment).code = 303;
	(*segment).msg_content = (char *) malloc(WRITE_SIZE);
	sprintf((*segment).msg_content, "%s", "USERINTERFACE");
	(*segment).length = strlen((*segment).msg_content);
}
