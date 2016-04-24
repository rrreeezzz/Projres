
//Parseur de commandes de retour
nbpossibleReturnData = 15;
char possibleReturnData[nbData][30] = {
                                  //Protocole de retour applicatif

                                  //Utilitaires
                                  "HELPDISPLAY", //Code 0
                                  "QUITCONFIRM", //Code 1

                                  //Gestion des contacts
                                  "ADDCONTACTCONFIRM", //Code 2
                                  "REMOVECONTACTCONFIRM", //Code 3
                                  "CONTACTLIST", //Code 4

                                  //Gestion des connections
                                  "WHOISCONNECTED", //Code 5
                                  "DISCONNECTCONFIRM", //Code 6
                                  "MSGCONFIRM", //Code 7
                                  "ALLMSGCONFIRM", //Code 8

                                  //Evenement de deconnection
                                  "USERDISCONNECTED", // Code 9
                                  "USERASKCONNECT", //Code 10

                                  //Gestion de transfert
                                  "TRANSFERINITATECONFIRM", //Code 11
                                  "TRANSFERACCEPTCONFIRM", //Code 12
                                  "TRANSFERREFUSECONFIRM", //Code 13

                                  //Evenements de transfert
                                  "TRANSFERACCEPTASK", //Code 14
                                  "TRANSFERREFUSE" //Code 15
                                }

int connectClient(){
  int sock_host;
  struct hostent *hostinfo;
  struct sockaddr_in address;

  sock_host = socket(AF_INET, SOCK_STREAM, 0);

  address.sin_addr.s_addr = inet_addr(adresseClientPrincipal);
  address.sin_family = AF_INET;
  address.sin_port = htons(portClientPrincipal);

  return connect(sock_host, (struct sockaddr *)&address, sizeof(address));
}

int parseData(char * data){
  //parse la commande

  int i;

  for (i = 0; i < nbData; i++) {
    if (strncmp(data, possibleData[i], strlen(possibleData[i]) ) == 0){
      switch (i) {

      }
      break; //On evite de parcourir la fin du tableau
    }
  }
}
