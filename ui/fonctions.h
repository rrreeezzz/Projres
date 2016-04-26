#include "utilities.h"
//Fonctions executees

/*
* Gestion des contacts
*/
void on_add_contact(char * username){
  printf(RED"%s "BLUE"ajouté !"RESET"\n",username);
  addContact(username);
}

void on_update_contact(char * username){
  printf(RED"%s "BLUE"mis à jour !"RESET"\n",username );
}

void on_remove_contact(char * username){
  printf(RED"%s "BLUE"supprimé !"RESET"\n",username );
  removeContact(username);
}

//Mise a jour de la liste des contacts
void on_contact_list_update(int length){
  printf(BLUE"\nMise a jour de la liste des contacts: "RED"%d elements."RESET"\n",length );
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
  printf(BLUE"Fin de mise a jour de la liste des contacts !"RESET"\n\n");
  int i;
  for (i = 0; i < MAXCONTACTS; i++) {
    if (contactArray[i].flag == 0 && contactArray[i].name != NULL){
      removeContact(contactArray[i].name);
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
    printf(BLUE"\nMise a jour de la liste des connections: "RED"%d elements."RESET"\n",length );
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
  printf(BLUE"Fin de mise a jour de la liste des connections !"RESET"\n\n");
  int i;
  for (i = 0; i < MAXTABS; i++) {
    if (tabs[i].flag == 0 && tabs[i].name != NULL){
      closeTab(tabs[i].name);
      return;
    }
  }
}

/*
* Gestion des messages
*/
void on_message_recv(char * username, char * content){
  printf(BLUE"Message de "RED"%s"BLUE" : "RED"%s"RESET"\n",username,content);
  int i;
  for (i = 0; i < MAXTABS; i++) {
    if ( tabs[i].name!=NULL && strcmp(tabs[i].name,username) == 0) {
      char ligne[WRITE_SIZE];
      sprintf(ligne,"%s: %s",username,content);
      write_line_text_zone(tabs[i].textZone,ligne);
      return;
    }
  }

}
void on_message_send(char * username, char * content){
  printf(BLUE"Message envoyé à"RED" %s"BLUE" : "RED"%s"RESET"\n",username,content);
  int i;
  for (i = 0; i < MAXTABS; i++) {
    if ( tabs[i].name!=NULL && strcmp(tabs[i].name,username) == 0) {
      char ligne[WRITE_SIZE];
      sprintf(ligne,"me: %s",content);
      write_line_text_zone(tabs[i].textZone,ligne);
      return;
    }
  }
}

void on_message_sendall(char * content){
  printf(BLUE"Message envoyé à tous : "RED"%s"RESET"\n",content);
}

/*
* Serveur de contact
*/
void on_online_server(){
  printf(BLUE"Serveur de contact en ligne !"RESET"\n");
  serverState = 1;
  switch_server_update();
}
void on_offline_server(){
  printf(BLUE"Serveur de contact hors ligne !"RESET"\n");
  serverState = 0;
  switch_server_update();
}

/*
* Gestion des erreurs
*/
void on_message_error(char * content){
  printf(BLUE"Erreur d'envoi de message : "RED"%s"RESET"\n",content);
  char messsge[WRITE_SIZE];
  sprintf(messsge,"Erreur d'envoi de message : %s\n",content);
  quick_message(GTK_WINDOW(window), messsge);
}
void on_add_contact_error(char * content){
  printf(BLUE"Erreur d'ajout de contact : "RED"%s"RESET"\n",content);
  char messsge[WRITE_SIZE];
  sprintf(messsge,"Erreur d'ajout de contact : %s\n",content);
  quick_message(GTK_WINDOW(window), messsge);
}
void on_remove_contact_error(char * content){
  printf(BLUE"Erreur de supression de contact : "RED"%s"RESET"\n",content);
  char messsge[WRITE_SIZE];
  sprintf(messsge,"Erreur de supression de contact : %s\n",content);
  quick_message(GTK_WINDOW(window), messsge);
}
void on_server_error(char * content){
  printf(BLUE"Erreur du serveur de contact : "RED"%s"RESET"\n",content);
  char messsge[WRITE_SIZE];
  sprintf(messsge,"Erreur du serveur de contact : %s\n",content);
  quick_message(GTK_WINDOW(window), messsge);
}

void on_connect_error(char * content){
  printf(BLUE"Impossible de se connecter : "RED"%s"RESET"\n",content);
  char messsge[WRITE_SIZE];
  sprintf(messsge,"Impossible de se connecter : %s\n",content);
  quick_message(GTK_WINDOW(window), messsge);
}
