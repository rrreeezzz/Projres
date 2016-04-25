//Retourne un num de tab non prit
int opt_tab(){
  int i;
  for (i = 0; i < MAXTABS-1; i++) {
    if (tabs[i].grid==NULL) return i;
  }
  return -1;
}

//ecris une ligne dans une zone de texte selectionnee
void write_line_text_zone(GtkWidget * textZone, char * msg){
  GtkTextIter endIter;

  //On recupere le buffer
  GtkTextBuffer * textZone_buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW (textZone));
  //On recupere la derniere ligne du buffer
  gtk_text_buffer_get_end_iter (textZone_buf, &endIter);
  //On rentre une ligne
  gtk_text_buffer_insert(textZone_buf, &endIter ,(gchar *)msg,strlen(msg));
}

int opt_client(){
  int i;
  for (i = 0; i < MAXCONTACTS-1; i++) {
    if (contactArray[i].name==NULL) return i;
  }
  return -1;
}

void removeContact(char * name){
  int i;
  for (i = 0; i < MAXCONTACTS; i++) {
    if (strcmp(contactArray[i].name,name) == 0){
      gtk_widget_hide(GTK_WIDGET(contactArray[i].listElement));
      free(contactArray[i].name);
      contactArray[i].name=NULL;
      return;
    }
  }
}

void closeTab(char * name){
  int i;
  for (i = 0; i < MAXTABS; i++) {
    if (tabs[i].name != NULL && strcmp(tabs[i].name,name) == 0){
      gtk_widget_destroy(GTK_WIDGET(tabs[i].grid));
      free(tabs[i].name);
      tabs[i].name = NULL;
      return;
    }
  }
}

//Fermer une tab, ne pas appeler, c'est une fonction pour un signal
void close_connection_request(GtkButton *button, char * name){
  char request[WRITE_SIZE];
  sprintf(request,"/disconnect %s\n",name);
  sendRequest(request);
}

void removeContactSignal(GtkWidget *widget,char * name) {
  char request[WRITE_SIZE];
  sprintf(request,"/remove %s\n",name);
  sendRequest(request);
}

void connectToUser(GtkWidget *widget,char * name) {
  char request[WRITE_SIZE];
  sprintf(request,"/connect %s\n",name);
  sendRequest(request);
}

void send_private_message(GtkWidget *widget,int *nbTab){
  char request[WRITE_SIZE];
  sprintf(request,"/msg %s %s\n",tabs[*nbTab].name,gtk_entry_get_text(GTK_ENTRY(tabs[*nbTab].inputZone)));
  sendRequest(request);
  gtk_entry_set_text(GTK_ENTRY(tabs[*nbTab].inputZone),"");
}

void addContact(char * name){
  int idClient = opt_client();
  contactArray[idClient].name = malloc(16*sizeof(char));
  contactArray[idClient].flag = 1;

  strcpy(contactArray[idClient].name,name);

  //Label
  GtkWidget * labelName = gtk_label_new(name);

  //Boutons de connection
  GtkWidget * removeContact = gtk_button_new_with_label("X");
  GtkWidget * connectButton = gtk_button_new_with_label("=>");

  //Signaux
  g_signal_connect(G_OBJECT( removeContact ), "clicked", G_CALLBACK(removeContactSignal), contactArray[idClient].name);
  g_signal_connect(G_OBJECT( connectButton ), "clicked", G_CALLBACK(connectToUser), contactArray[idClient].name);

  //Creation de la grille
  GtkWidget * grilleDuContact = gtk_grid_new();
  gtk_grid_set_row_homogeneous(GTK_GRID(grilleDuContact),TRUE);
  gtk_grid_set_column_homogeneous(GTK_GRID(grilleDuContact),TRUE);

  //On remplis la grille
  gtk_grid_attach(GTK_GRID(grilleDuContact),removeContact,1,1,1,1);
  gtk_grid_attach(GTK_GRID(grilleDuContact),labelName,2,1,2,1);
  gtk_grid_attach(GTK_GRID(grilleDuContact),connectButton,4,1,1,1);

  contactArray[idClient].listElement = grilleDuContact;

  //Packing
  gtk_container_add(GTK_CONTAINER(contactList),GTK_WIDGET(grilleDuContact));
  gtk_widget_show_all(grilleDuContact);
}

/* Ajouter une tab, prend en parametre le nom,
type=1 si seul => Name est le nom du client
type=2 si groupe => Name est le nom du groupe
*/
int addTab(char * name){
  int nbTab;
  char buff[200];

  /* On cree les elements */
  GtkWidget * outputZone = gtk_text_view_new();
  GtkWidget * inputZone = gtk_text_view_new();
  GtkWidget * grid = gtk_grid_new();
  GtkWidget * label = gtk_label_new(name);
  GtkWidget * sendButton = gtk_button_new();

  /* On remplis le tableau */
  nbTab=opt_tab();
  tabs[nbTab].grid = grid;
  tabs[nbTab].label = label;
  tabs[nbTab].nbTab = nbTab;
  tabs[nbTab].name = malloc(16*sizeof(char));
  tabs[nbTab].flag = 1;
  tabs[nbTab].textZone = outputZone;

  strcpy(tabs[nbTab].name,name);

  /* Boutons de controle */
  GtkWidget * button = gtk_button_new();
  gtk_button_set_label((GtkButton *)button, "Close tab");
  g_signal_connect(G_OBJECT( button ), "clicked", G_CALLBACK(close_connection_request),tabs[nbTab].name);

  /* Zone de lecture de conversation */
  gtk_text_view_set_editable(GTK_TEXT_VIEW(outputZone),FALSE);
  g_object_set(G_OBJECT(outputZone), "margin", 20, NULL);
  sprintf(buff,"Connection etablished with: %s\n",name);
  write_line_text_zone(outputZone,buff);

  /* Zone d'ecriture */
  inputZone = gtk_entry_new();
  tabs[nbTab].inputZone = inputZone;

  /* Boutons d'envoi*/
  gtk_button_set_label((GtkButton *)sendButton, "Envoyer");
  g_signal_connect(G_OBJECT(sendButton), "clicked", G_CALLBACK(send_private_message),&tabs[nbTab].nbTab);

  /* Formation de la grille */
  gtk_grid_set_row_homogeneous(GTK_GRID(grid),TRUE);
  gtk_grid_set_column_homogeneous(GTK_GRID(grid),TRUE);

  //Imbrication   gtk_grid_attach (grid,child,left,top,width,height);
  gtk_grid_attach(GTK_GRID(grid),button,1,1,3,1);
  gtk_grid_attach(GTK_GRID(grid),outputZone,1,2,3,14);
  gtk_grid_attach(GTK_GRID(grid),inputZone,1,16,2,1);
  gtk_grid_attach(GTK_GRID(grid),sendButton,3,16,1,1);

  /*On cree la page du notebook */
  gtk_widget_set_name(GTK_WIDGET(label), "settings_tab");
  gtk_notebook_insert_page(GTK_NOTEBOOK(tabmenu),GTK_WIDGET(grid), GTK_WIDGET(label), -1);

  //On met tout a jour
  gtk_widget_show_all(grid);

  //on y va!
  gtk_notebook_set_current_page(GTK_NOTEBOOK(tabmenu),gtk_notebook_page_num(GTK_NOTEBOOK(tabmenu),grid));
  return 0;
}
