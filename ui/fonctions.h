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
