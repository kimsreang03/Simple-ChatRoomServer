#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_NAME 16
#define MAX_BUFF 1024

void check_err(int err, const char* fmt);
void recvfromserv();

int sockfd;
struct sockaddr_in server;
pthread_t thread;

int main(int argc, char* argv[]){

  if(argc != 3){
    printf("usage: %s <server_address> <port>\n", argv[0]);
    return 1;
  }

  

  bzero(&server, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY); 
  server.sin_port = htons(atoi(argv[2]));
  
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  check_err(sockfd, "failed to create socket!\n");

  check_err(connect(sockfd, (struct sockaddr*)&server, sizeof(server)), "failed to connect!");
  write(1, "\x1b[1;93m>>> successfully connected to the server.\n", 49);  

  //get name from user and send to server
  char buffer[MAX_BUFF+1];
  int n;

  write(1, "\x1b[1;4;37menter your name:\x1b[24m ", 31);  
  
  n = read(STDIN_FILENO, buffer, MAX_NAME); // get input name
  n--;
  buffer[n] = 0;

  send(sockfd, buffer, n, 0); //send to server
  printf("\x1b[1;93m>>> your name is set to '%s'\n", buffer);
  printf(">>> now you can chat with other connected user :)\n\n");
  fflush(stdout);

  pthread_create(&thread, NULL, (void*)recvfromserv, NULL);

  while(1){


    write(STDOUT_FILENO, "\x1b[1;93m>\x1b[0m ", 13);
    
    n = read(STDERR_FILENO, buffer, MAX_BUFF);
    buffer[n] = 0;
    if(!strcmp("exit\n", buffer)){
      // pthread_join(thread, NULL);
      close(sockfd);
      puts("\x1b[0m");
      return 0;
    }

    send(sockfd, buffer, n, 0);

  }

  return 0;
}

void recvfromserv(){

  char buffer[MAX_BUFF+1];
  int n;

  while(1){

    n = recv(sockfd, buffer, MAX_BUFF, 0);
    buffer[n] = 0;
    if(n > 0)
    write(1, buffer, n);
    write(STDOUT_FILENO, "\x1b[1;93m>\x1b[0m ", 13);


  }

}


void check_err(int err, const char* fmt){

  if(err   == -1){
    fprintf(stderr, "error: %s\n", fmt);
    exit(1);
  }

}// check()