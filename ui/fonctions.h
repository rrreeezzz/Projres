#include "utilities.h"
//Fonctions executees

/*
* Gestion des contacts
*/
void on_add_contact(char * username){
  printf(BLUE"%s added !"RESET"\n",username);
  addContact(username);
}

void on_update_contact(char * username){
  printf(BLUE"%s updated !"RESET"\n",username );
}

void on_remove_contact(char * username){
  printf(BLUE"%s removed !"RESET"\n",username );
  removeContact(username);
}

//Mise a jour de la liste des contacts
void on_contact_list_update(int length){
  printf(BLUE"Mise a jour de la liste des contacts: %d elements."RESET"\n",length );
  int i;
  for (i = 0; i < MAXCONTACTS; i++) {
    contactArray[i].flag = 0;
    return;
  }
}

void on_contact_list_iteration(char * username){
  printf(BLUE"Contact de la liste: "RED"%s"RESET"\n",username);
  int i;
  for (i = 0; i < MAXCONTACTS; i++) {
    if ( contactArray[i].name!=NULL && strcmp(contactArray[i].name,username) == 0) {
      contactArray[i].flag = 1;
      return;
    }
  }
  addContact(username);
}

void on_contact_list_end(){
  printf(BLUE"Fin de mise a jour de la liste des contacts !"RESET"\n");
  int i;
  for (i = 0; i < MAXCONTACTS; i++) {
    if (contactArray[i].flag == 0 && contactArray[i].name != NULL){
      printf(GREEN"contact %s\n"RESET, contactArray[i].name);
      return;
    }
  }
}

/*
* Gestion des connections
*/
void on_deconnection(char * username){
  printf(BLUE"Déconnection de: "RED"%s"RESET"\n",username);
  closeTab(username);
}

void on_connection_ask(char * username, char * adress){
  printf(BLUE"Demande de connection de la part de: "RED"%s"BLUE" à l'adresse "RED"%s"RESET"\n",username,adress);
  ask_connect(GTK_WINDOW(window),username,adress);
}

void on_connection_confirm(char * username){
  printf(BLUE"Connection avec "RED"%s"BLUE" établie"RESET"\n",username);
  addTab(username);
}
//Mise a jour de des connections
void on_connection_list_update(int length){
    printf(BLUE"Mise a jour de la liste des connections: %d elements."RESET"\n",length );
    int i;
    for (i = 0; i < MAXTABS; i++) {
      tabs[i].flag = 0;
      return;
    }
}

void on_connection_list_iteration(char * username){
  printf(BLUE"Connection: "RED"%s"RESET"\n",username);
  int i;
  for (i = 0; i < MAXTABS; i++) {
    if ( tabs[i].name!=NULL && strcmp(tabs[i].name,username) == 0) {
      tabs[i].flag = 1;
      return;
    }
  }
  addTab(username);
}

void on_connection_list_end(){
  printf(BLUE"Fin de mise a jour de la liste des connections !"RESET"\n");
  int i;
  for (i = 0; i < MAXTABS; i++) {
    if (tabs[i].flag == 0 && tabs[i].name != NULL){
      printf(GREEN"contact %s\n"RESET, tabs[i].name);
      return;
    }
  }
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
