#include "ui.h"

int main(int argc, char *argv[] ) {
  gtk_init (&argc, &argv);

  //Generation fenetre
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size(GTK_WINDOW(window), windowHeight, windowWidth);
  g_signal_connect (GTK_WIDGET (window), "destroy", G_CALLBACK (gtk_main_quit), NULL);

  //Grille principale
  GtkWidget * mainGrid = gtk_grid_new();

  //Grille connection contacts
  GtkWidget * contactGrid = gtk_grid_new();
  contactList = gtk_list_box_new();
  GtkWidget * gtk_scrolled_window_new(GtkAdjustment *hadjustment,GtkAdjustment *vadjustment);


  //Creation tab
  tabmenu = gtk_notebook_new ();
  gtk_widget_set_name(GTK_WIDGET(tabmenu), "Conversations");
  gtk_notebook_set_tab_pos(GTK_NOTEBOOK (tabmenu), GTK_POS_TOP);
  gtk_notebook_set_scrollable(GTK_NOTEBOOK(tabmenu),TRUE); //Rendre scrollable horizontallement


  /*
  GtkWidget * button = gtk_button_new();
  gtk_button_set_label((GtkButton *)button, "Add a contact");
  */
  GtkWidget * contactListTitle=gtk_label_new("Contacts");
  gtk_list_box_set_activate_on_single_click(GTK_LIST_BOX(contactList),TRUE);
  gtk_widget_set_margin_top(GTK_WIDGET(contactListTitle),10);
  gtk_widget_set_margin_bottom(GTK_WIDGET(contactListTitle),10);

  GtkWidget * addContact=gtk_label_new("Ajouter contact");
  gtk_widget_set_margin_top(GTK_WIDGET(addContact),10);
  gtk_widget_set_margin_bottom(GTK_WIDGET(addContact),10);

  GtkWidget * refreshContact=gtk_label_new("Rafraichir");
  gtk_widget_set_margin_top(GTK_WIDGET(refreshContact),10);
  gtk_widget_set_margin_bottom(GTK_WIDGET(refreshContact),10);

  g_signal_connect(GTK_WIDGET(contactList), "row-activated", G_CALLBACK(enter_adress),"Hello");

  gtk_grid_set_row_homogeneous(GTK_GRID(contactGrid),TRUE);
  gtk_grid_set_column_homogeneous(GTK_GRID(contactGrid),TRUE);

  gtk_list_box_insert(GTK_LIST_BOX(contactList),GTK_WIDGET(addContact),0);
  gtk_list_box_insert(GTK_LIST_BOX(contactList),GTK_WIDGET(refreshContact),1);
  //gtk_container_add(GTK_CONTAINER(contactGrid),contactList);
  gtk_grid_attach(GTK_GRID(contactGrid),contactListTitle,1,1,1,1);
  gtk_grid_attach(GTK_GRID(contactGrid),contactList,1,2,1,20);

  //Tab positionnement de la denetre principale
  GtkWidget * windowGrid = gtk_grid_new();
  gtk_grid_set_row_homogeneous(GTK_GRID(windowGrid),TRUE);
  gtk_grid_set_column_homogeneous(GTK_GRID(windowGrid),TRUE);

  //Tab principal declare en global
  GtkWidget * contact_label = gtk_label_new("Main");
  gtk_widget_set_name(GTK_WIDGET(contact_label), "main_tab");

  //Imbrication
  gtk_container_add(GTK_CONTAINER(window),windowGrid);
  gtk_grid_attach(GTK_GRID(windowGrid),tabmenu,4,1,9,1);
  gtk_grid_attach(GTK_GRID(windowGrid),contactGrid,1,1,3,1);

  gtk_container_add(GTK_CONTAINER(tabmenu),mainGrid);
  gtk_notebook_set_tab_label(GTK_NOTEBOOK(tabmenu),mainGrid, contact_label);

  //Boite de dialogue de connection
  connect_client_dialog(GTK_WINDOW(window));

  //Afficher fenetre puis main loop
  gtk_widget_show_all(window);

  int continueProgram = 1;
  int resultRoutine;
  while (continueProgram){
    while (gtk_events_pending ()) {
      gtk_main_iteration();
    }
    //si on est toujours connecte
    resultRoutine = routine_client(MINIMALUI_NOSTDIN);
    //rupture demandee de la part du client
    if (resultRoutine == 1){
      continueProgram = 0;
    //Rupture non demandee, on tente de se reconnecter
    } else if (resultRoutine == -1){
      connect_client_dialog(GTK_WINDOW(window));
    }
  }
  return(EXIT_SUCCESS);
}
