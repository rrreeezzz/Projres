// Display a quick message
void quick_message(GtkWindow *parent, gchar *message){
 GtkWidget *dialog, *label, *content_area;
 GtkDialogFlags flags;

 //Create the widgets
 flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
 dialog = gtk_dialog_new_with_buttons ("Message",GTK_WINDOW(window),flags,"OK",GTK_RESPONSE_NONE,NULL);
 content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
 label = gtk_label_new (message);

 //Ensure that the dialog box is destroyed when the user responds
 g_signal_connect_swapped(dialog,"response",G_CALLBACK (gtk_widget_destroy),dialog);

 //Add the label, and show everything we’ve added
 gtk_container_add (GTK_CONTAINER (content_area), label);
 gtk_widget_show_all (dialog);
}

// Display a quick message
void ask_connect(GtkWindow *parent, char *name,char * adress){
 GtkWidget *dialog, *label, *content_area;
 GtkDialogFlags flags;

 char contenu[200];
 char request[MSG_SIZE];

 sprintf(contenu,"%s essaie de se connecter depuis l'adresse %s.",name,adress);

 //Create the widgets
 flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
 dialog = gtk_dialog_new_with_buttons (contenu,GTK_WINDOW(window),flags,"Accepter",1,"Refuser",0,NULL);
 content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
 label = gtk_label_new(contenu);

 //Add the label, and show everything we’ve added
 gtk_container_add (GTK_CONTAINER (content_area), label);
 gtk_widget_show_all (dialog);

 int validorquit = 0;
 int result;
 while (validorquit == 0){
   result = gtk_dialog_run(GTK_DIALOG(dialog));
   printf("%d\n", result);
   if (result == 1) {
      validorquit = 1;
      sprintf(request,"/accept %s\n",name);
      sendRequest(request);
    }else{
      validorquit = 1;
      sprintf(request,"/refuse %s\n",name);
      sendRequest(request);
    }
  }
  gtk_widget_destroy(dialog);
}

void show_widget(GtkWindow *parent, GtkWidget * widget){
  gtk_widget_set_visible(GTK_WIDGET(widget) , !gtk_widget_get_visible(GTK_WIDGET(widget)));
}

void refresh_client_data(){
  sendRequest("/contact\n");
  sendRequest("/who\n");
}

//Entrer une adresse
void connect_client_dialog(GtkWindow *parent){
 GtkWidget *dialog,*labelAdress,*labelPort,*content_area,*entryAdress,*entryPort,*buttonAddr,*buttonPort;
 GtkDialogFlags flags;
 char port[10];

 //Create the widgets
 flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
 dialog = gtk_dialog_new_with_buttons ("Connection au client",GTK_WINDOW(window),flags,"OK",1,"CANCEL",0,NULL);
 content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

 //Create labels
 labelAdress = gtk_label_new("Adresse:");
 labelPort = gtk_label_new("Numéro de port:");

 //Create check buttons
 buttonAddr = gtk_check_button_new_with_label("Adresse locale");
 buttonPort = gtk_check_button_new_with_label("Port par default");

 //Active by default
 gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(buttonAddr) , TRUE);

 //Create inputs
 entryAdress = gtk_entry_new();
 entryPort = gtk_entry_new();
 gtk_entry_set_max_length(GTK_ENTRY(entryAdress),15);

 //signal checkboxes
 g_signal_connect(G_OBJECT( buttonAddr ), "clicked", G_CALLBACK(show_widget), GTK_WIDGET(entryAdress) );
 g_signal_connect(G_OBJECT( buttonPort ), "clicked", G_CALLBACK(show_widget), GTK_WIDGET(entryPort) );

 //Packing
 gtk_container_add(GTK_CONTAINER(content_area), labelAdress);
 gtk_container_add(GTK_CONTAINER(content_area), buttonAddr);
 gtk_container_add(GTK_CONTAINER(content_area), entryAdress);
 gtk_container_add(GTK_CONTAINER(content_area), labelPort);
 gtk_container_add(GTK_CONTAINER(content_area), buttonPort);
 gtk_container_add(GTK_CONTAINER(content_area), entryPort);

 //Veleurs par defaut
 sprintf(port,"%d",DEFAULTPORT);
 gtk_entry_set_text(GTK_ENTRY(entryAdress),DEFAULTADRESS);
 gtk_entry_set_text(GTK_ENTRY(entryPort),port);

 gtk_widget_show_all(dialog);

 //On cache l'adresse par default
 gtk_widget_set_visible(GTK_WIDGET(entryAdress),FALSE);

 int validorquit = 0;
 while (validorquit == 0){
   switch(gtk_dialog_run(GTK_DIALOG(dialog))){
    case 1:
      validorquit = 1;

      if (strlen(gtk_entry_get_text(GTK_ENTRY(entryAdress))) < 7 && !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(buttonAddr)) ){
        validorquit = 0;
      }
      if(!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(buttonPort)) && (atoi(gtk_entry_get_text(GTK_ENTRY(entryPort))) <= 0 || atoi(gtk_entry_get_text(GTK_ENTRY(entryPort))) >= 100000)){
        validorquit = 0;
      }

      //Si valide on applique une action
      if (validorquit == 1){
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(buttonAddr))){
          memcpy(adresseClientPrincipal,DEFAULTADRESS,strlen(DEFAULTADRESS));
        } else {
          memcpy (adresseClientPrincipal,gtk_entry_get_text(GTK_ENTRY(entryAdress)),strlen(gtk_entry_get_text(GTK_ENTRY(entryAdress))));
        }

        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(buttonPort))){
          portClientPrincipal = DEFAULTPORT;
        } else {
          portClientPrincipal = atoi(gtk_entry_get_text(GTK_ENTRY(entryPort)));
        }
      }
      break;
    case 0:
      exit(EXIT_SUCCESS);
    default:
        validorquit = 1;
        break;
    }
    printf("Connection à %s:%d\n",
      adresseClientPrincipal,
      portClientPrincipal
    );
    //Si on arrive pas a se connecter
    if (connect_client(MINIMALUI_NOSTDIN) == 0){
      refresh_client_data();
    } else {
      validorquit = 0;
      quick_message(GTK_WINDOW(parent), "Impossible de se connecter, veuillez réessayer.");
      printf("Erreur de connection\n");
    }

  }


  gtk_widget_destroy(dialog);
}

//Entrer une adresse
void enter_adress(GtkWindow *parent){
  GtkWidget *buttonServer,*dialog,*labelName,*labelAdress,*labelPort,*content_area,*entryAdress,*entryPort,*entryName;
  GtkDialogFlags flags;
  switch(gtk_list_box_row_get_index(gtk_list_box_get_selected_row(GTK_LIST_BOX(contactList)))){
    case 1:
      refresh_client_data();
      break;
    case 0:
      //Create labels
      labelName = gtk_label_new("Name (3-15 char):");
      labelAdress = gtk_label_new("Adress:");
      labelPort = gtk_label_new("Port number:");

      //Create inputs
      entryName = gtk_entry_new();
      entryAdress = gtk_entry_new();
      entryPort = gtk_entry_new();
      gtk_entry_set_max_length(GTK_ENTRY(entryName),15);
      gtk_entry_set_max_length(GTK_ENTRY(entryAdress),15);

      //Create the widgets
      flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
      dialog = gtk_dialog_new_with_buttons ("Add contact",GTK_WINDOW(window),flags,"OK",1,"Cancel",0,NULL);
      content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

      //Create check buttons
      buttonServer = gtk_check_button_new_with_label("Chercher sur le serveur d'annuaire");

      g_signal_connect(G_OBJECT( buttonServer ), "clicked", G_CALLBACK(show_widget), GTK_WIDGET(entryAdress) );
      g_signal_connect(G_OBJECT( buttonServer ), "clicked", G_CALLBACK(show_widget), GTK_WIDGET(entryPort) );
      g_signal_connect(G_OBJECT( buttonServer ), "clicked", G_CALLBACK(show_widget), GTK_WIDGET(labelAdress) );
      g_signal_connect(G_OBJECT( buttonServer ), "clicked", G_CALLBACK(show_widget), GTK_WIDGET(labelPort) );

      //Packing
      gtk_container_add(GTK_CONTAINER(content_area), labelName);
      gtk_container_add(GTK_CONTAINER(content_area), entryName);
      gtk_container_add(GTK_CONTAINER(content_area), buttonServer);
      gtk_container_add(GTK_CONTAINER(content_area), labelAdress);
      gtk_container_add(GTK_CONTAINER(content_area), entryAdress);
      gtk_container_add(GTK_CONTAINER(content_area), labelPort);
      gtk_container_add(GTK_CONTAINER(content_area), entryPort);

      gtk_widget_show_all(dialog);

      int validorquit = 0;
      while (validorquit == 0){
        switch(gtk_dialog_run(GTK_DIALOG(dialog))){
         case 1:
           validorquit = 1;

           //Validation par flag
           if (strlen(gtk_entry_get_text(GTK_ENTRY(entryName))) < 3 || strlen(gtk_entry_get_text(GTK_ENTRY(entryName))) > 15){
             quick_message(GTK_WINDOW(dialog),"Le nom doit être entre 3 et 15 caracteres.");
             validorquit = 0;
           }

           if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(buttonServer))){
             if (strlen(gtk_entry_get_text(GTK_ENTRY(entryAdress))) < 7){
               quick_message(GTK_WINDOW(dialog),"Adresse invalide.");
               validorquit = 0;
             }
             if(atoi(gtk_entry_get_text(GTK_ENTRY(entryPort))) <= 0 || atoi(gtk_entry_get_text(GTK_ENTRY(entryPort))) >= 100000){
               quick_message(GTK_WINDOW(dialog),"Le port doit être entre 0 et 100000");
               validorquit = 0;
             }

             //Si valide on ajoute un contact
             if (validorquit == 1){
                 char request[MSG_SIZE];
                 sprintf( request,"/add %s %s:%d \n",gtk_entry_get_text(GTK_ENTRY(entryName)),gtk_entry_get_text(GTK_ENTRY(entryAdress)),atoi(gtk_entry_get_text(GTK_ENTRY(entryPort))) );
                 sendRequest(request);
             }
           } else {
             //Si valide on ajoute un contact
             if (validorquit == 1){
                 char request[MSG_SIZE];
                 sprintf( request,"/search %s\n",gtk_entry_get_text(GTK_ENTRY(entryName)));
                 sendRequest(request);
             }
           }

           break;
         default:
             validorquit = 1;
             break;
         }
       }


       gtk_widget_destroy(dialog);

    break;
    default:
      break;
    }
}
