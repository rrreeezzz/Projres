#include "client.c"

int main(int argc, char const *argv[]) {
  switch(fork()){
    case 0:
    printf ("Am child, my ID: %d", getpid() );
    server();
    break;

    case -1:
    printf ("The child process has not created");
    break;

    default:
    printf ("am in default , process id: %d",
    client();
    break;
  }
  return 0;
}
