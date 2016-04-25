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
  char * name;
  int nbTab;
  int flag;
  GtkWidget * textZone;
  GtkWidget * inputZone;
} onglet;

//Pour afficher les clients du carnet
typedef struct contactData {
  char * name;
  GtkWidget * listElement;
  int flag;
} contactData;

onglet tabs[MAXTABS];
contactData contactArray[MAXCONTACTS];

int routine_client();
int connect_client();
int sendRequest(char * content);

#define WRITE_SIZE 1000
#define MSG_SIZE WRITE_SIZE+25
#define MAX_SIZE_PORT 5
#define MIN_SIZE_USERNAME 4
#define MAX_SIZE_USERNAME 16
#define MAX_SIZE_ADDRESS 22

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "tabManagement.c"
#include "dialog.c"

// Fonctions.h contient les differentes fonctions lies aux evenements du serveur
#include "fonctions.h"

// Librairie
#include "MinimalUI/lib/minimalUI.h"
