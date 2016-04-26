#include "utilities.h"
//Fonctions executees

/*
* Gestion des contacts
*/
void on_add_contact(char * username){
  printf(BLUE"%s added !"RESET"\n",username );
}

void on_update_contact(char * username){
  printf(BLUE"%s updated !"RESET"\n",username );
}

void on_remove_contact(char * username){
  printf(BLUE"%s removed !"RESET"\n",username );
}

//Mise a jour de la liste des contacts
void on_contact_list_update(int length){
  printf(BLUE"Mise a jour de la liste des contacts: %d elements."RESET"\n",length );
}

void on_contact_list_iteration(char * username){
  printf(BLUE"Contact de la liste: "RED"%s"RESET"\n",username);
}

void on_contact_list_end(){
  printf(BLUE"Fin de mise a jour de la liste des contacts !"RESET"\n");
}

/*
* Gestion des connections
*/
void on_deconnection(char * username){
  printf(BLUE"Déconnection de: "RED"%s"RESET"\n",username);
}

void on_connection_ask(char * username, char * adress){
  printf(BLUE"Demande de connection de la part de: "RED"%s"BLUE" à l'adresse "RED"%s"RESET"\n",username,adress);
}

void on_connection_confirm(char * username){
  printf(BLUE"Connection avec "RED"%s"BLUE" établie"RESET"\n",username);
}
//Mise a jour de des connections
void on_connection_list_update(int length){
    printf(BLUE"Mise a jour de la liste des connections: %d elements."RESET"\n",length );
}

void on_connection_list_iteration(char * username){
  printf(BLUE"Connection: "RED"%s"RESET"\n",username);
}

void on_connection_list_end(){
  printf(BLUE"Fin de mise a jour de la liste des connections !"RESET"\n");
}

/*
* Gestion des messages
*/
void on_message_recv(char * username, char * content){
  printf(BLUE"Message de "RED"%s"BLUE" : "RED"%s"RESET"\n",username,content);
}
void on_message_send(char * username, char * content){
  printf(BLUE"Message envoyé à"RED" %s"BLUE" : "RED"%s"RESET"\n",username,content);
}
void on_message_sendall(char * content){
  printf(BLUE"Message envoyé à tous : "RED"%s"RESET"\n",content);
}

/*
* Serveur de contact
*/
void on_online_server(){
  printf(BLUE"Serveur de contact en ligne !"RESET"\n");
}
void on_offline_server(){
  printf(BLUE"Serveur de contact hors ligne !"RESET"\n");
}

/*
* Gestion des erreurs
*/
void on_message_error(char * content){
  printf(BLUE"Erreur d'envoi de message : "RED"%s"RESET"\n",content);
}
void on_add_contact_error(char * content){
  printf(BLUE"Erreur d'ajout de contact : "RED"%s"RESET"\n",content);
}
void on_remove_contact_error(char * content){
  printf(BLUE"Erreur de supression de contact : "RED"%s"RESET"\n",content);
}
void on_server_error(char * content){
  printf(BLUE"Erreur du serveur de contact : "RED"%s"RESET"\n",content);
}
void on_connect_error(char * content){
  printf(BLUE"Impossible de se connecter : "RED"%s"RESET"\n",content);
}
