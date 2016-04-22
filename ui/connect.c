int connectClient(){
  int sock_host;
  struct hostent *hostinfo;
  struct sockaddr_in address;

  sock_host = socket(AF_INET, SOCK_STREAM, 0);

  address.sin_addr.s_addr = inet_addr(adressClientPrincipal);
  address.sin_family = AF_INET;
  address.sin_port = htons(portClientPrincipal);

  return connect(sock_host, (struct sockaddr *)&address, sizeof(address));
}
