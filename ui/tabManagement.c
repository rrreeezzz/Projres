//Retourne un num de tab non prit
int opt_tab(){
  int i;
  for (i = 0; i < MAXTABS-1; i++) {
    if (tabs[i].grid==NULL) return i;
  }
  return -1;
}

//Fermer une tab, ne pas appeler, c'est une fonction pour un signal
static void close_tab(GtkButton *button, onglet * onglet){
  gint page;

  page = gtk_notebook_page_num(GTK_NOTEBOOK(tabmenu),onglet->grid);
  gtk_notebook_remove_page(GTK_NOTEBOOK(tabmenu), page);

  onglet->grid = NULL;
}

//ecris une ligne dans une zone de texte selectionnee
void write_line_text_zone( GtkWidget * textZone, char * msg){
  GtkTextIter endIter;

  //On recupere le buffer
  GtkTextBuffer * textZone_buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW (textZone));
  //On recupere la derniere ligne du buffer
  gtk_text_buffer_get_end_iter (textZone_buf, &endIter);
  //On rentre une ligne
  gtk_text_buffer_insert(textZone_buf, &endIter ,(gchar *)msg,strlen(msg));
}

/* Ajouter une tab, prend en parametre le nom,
type=1 si seul => Name est le nom du client
type=2 si groupe => Name est le nom du groupe
*/
int addTab(char * name,int type){
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
  tabs[nbTab].type = type;
  tabs[nbTab].nbTab = nbTab;

  /* Boutons de controle */
  GtkWidget * button = gtk_button_new();
  gtk_button_set_label((GtkButton *)button, "Close tab");
  g_signal_connect(G_OBJECT( button ), "clicked", G_CALLBACK(close_tab), &tabs[nbTab] );

  /* Zone de lecture de conversation */
  gtk_text_view_set_editable(GTK_TEXT_VIEW(outputZone),FALSE);
  g_object_set(G_OBJECT(outputZone), "margin", 20, NULL);
  sprintf(buff,"Connection etablished with: %s",name);
  write_line_text_zone(outputZone,buff);

  /* Zone d'ecriture */
  inputZone = gtk_entry_new();

  /* Boutons d'envoi*/
  gtk_button_set_label((GtkButton *)sendButton, "Envoyer");

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
