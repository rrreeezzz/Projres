void parseApplicationMessage(char * message){
  char * name;
  char * adress;
  char * content;
  *(message+strlen(message)-1)='\0';

  /*
  * Gestion des contacts
  */
  if (strncmp(message, "ADDCONTACTCONFIRM ", 18 ) == 0){
    on_add_contact(message+18);
  }

  if (strncmp(message, "UPDATECONTACTCONFIRM ", 21 ) == 0){
    on_update_contact(message+21);
  }

  if (strncmp(message, "REMOVECONTACTCONFIRM ", 21 ) == 0){
    on_remove_contact(message+21);
  }

  //mise a jour de la liste des contacts
  if (strncmp(message, "CONTACTLIST ", 12 ) == 0){
    on_contact_list_update(atoi(message+12));
    contact_list_position = 0;
    nb_elems_contact_list = atoi(message+12);
    if (contact_list_position == nb_elems_contact_list){
      on_contact_list_end();
    }
  }

  if (strncmp(message, "CONTACT ", 8 ) == 0){
    on_contact_list_iteration(message+8);
    contact_list_position++;
    if (contact_list_position == nb_elems_contact_list){
      on_contact_list_end();
    }
  }

  //mise a jour de la liste des contacts
  if (strncmp(message, "WHOISCONNECTED ", 15 ) == 0){
    on_connection_list_update(atoi(message+15));
    connected_list_position = 0;
    nb_elems_connected_list = atoi(message+15);
    if (connected_list_position == nb_elems_connected_list){
      on_connection_list_end();
    }
  }

  if (strncmp(message, "CONNECTED ", 10 ) == 0){
    on_connection_list_iteration(message+10);
    connected_list_position++;
    if (connected_list_position == nb_elems_connected_list){
      on_connection_list_end();
    }
  }

  if (strncmp(message, "DISCONNECTCONFIRM ", 18 ) == 0){
    on_deconnection(message+18);
  }

  if (strncmp(message, "CONNECTIONCONFIRM ", 18 ) == 0){
    on_connection_confirm(message+18);
  }

  if (strncmp(message, "CONNECTIONASK ", 14 ) == 0){
    name = strtok(message+14, " ");
    adress = strtok(NULL, "\0");
    on_connection_ask(name,adress);
  }

  if (strncmp(message, "MSGRECV ", 8 ) == 0){
    name = strtok(message+8, " ");
    content = strtok(NULL, "\0");
     on_message_recv(name,content);
  }

  if (strncmp(message, "MESSAGECONFIRM ", 15 ) == 0){
    name = strtok(message+15, " ");
    content = strtok(NULL, "\0");
    on_message_send(name,content);
  }

  if (strncmp(message, "ALLMSGCONFIRM ", 14 ) == 0){
    on_message_sendall(message+14);
  }
}
