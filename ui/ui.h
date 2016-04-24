#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

int connectClient();

#define DEFAULTADRESS "127.0.0.1"
#define DEFAULTPORT 46100

#define MAXTABS 100 //Nb max tab
#define MAXGROUP 100 //Nb max users per group
#define MAXCONTACTS 100 //Nb max de contacts

int windowHeight=800;
int windowWidth=600;

//Fenetre principale
GtkWidget * tabmenu;
GtkWidget * window;
GtkWidget * contactList;

//Connection principale au client
char adresseClientPrincipal[16];
int portClientPrincipal;
int fdPrincipal;

//Structure de gestion des onglets
typedef struct onglet {
  GtkWidget * grid;
  GtkWidget * label;
  int type; //1 si direct, 2 si groupe unicast
  int fd;
  int nbTab;
} onglet;

//Pour afficher les clients du carnet
typedef struct clientData {
  char * name;
  char * adress;
  int port;
} clientData;

onglet tabs[MAXTABS];
clientData clientsArray[MAXCONTACTS];

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "tabManagement.c"
#include "dialog.c"
#include "communication.c"
