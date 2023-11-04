#include "server.h"

int cli_count = 0; 
int uid = 1; // id for each client

Client* clients[MAX_CLI];


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int listenfd, connfd;
struct sockaddr_in server;
pthread_t thread;


void when_interupt(){
  
  for(int i = 0 ; i < MAX_CLI; i++){
    if(clients[i]){
      close(clients[i]->sockfd);
      free(clients[i]);
    }
  }

  close(listenfd);
  sleep(2);
  exit(0);
}


int main(int argc, char* argv[]){
  

  if(argc != 2){
    printf("usage: %s <port>\n", argv[0]);
    return 0;
  }

  int server_port = atoi(argv[1]);

  signal(SIGINT, when_interupt);
  //zero the server
  bzero(&server, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port = htons(server_port);

  // create socket 
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  check_err(listenfd, "socket failed!");

  //bind and listen
  check_err(bind(listenfd, (struct sockaddr*)&server, sizeof(server)), "bind failed!");
  check_err(listen(listenfd, MAX_CLI), "listen failed!");

  printf("waiting for connection on port %d ...\n", server_port);
  fflush(stdout);

  
  while(1){
    //keep accepting for new connection in the main thread
    connfd = accept(listenfd, NULL, NULL);

    // if there are already 10 clients connected, reject other connection
    if((cli_count+1) == MAX_CLI){
      printf("the server is full :)\n");
      close(connfd);
      continue;
    }

    Client* cli = malloc(sizeof(Client));
    cli->sockfd = connfd;
    cli->uid = uid++;

    add_client(cli);
    pthread_create(&thread, NULL, handling_client, (void*)cli);
    cli->tid = thread;
    
    sleep(1);
  }


 return 0;
}

void cutnewline(char* buff){
  for(int i = 0; i < 16; i++){
    if(buff[i] == '\n'){
      buff[i] = 0;
      break;
    }
  }
}

void* handling_client(void* cli){


  Client* client = (Client*) cli;
  char buffer[MAX_BUFF+1];
  int n;
  int leave_flag = 0;


  if((n = recv(client->sockfd, buffer, 32, 0)) <= 0){
    leave_flag = 1;
  }else{
    strncpy(client->name, buffer, n);
    printf("%s has joined the server.\n", client->name);
    fflush(stdout);
    send_message(client->uid, "%s has joined the server.\n", client->name);
  }

  while(1){
    if(leave_flag) break;

    int receive = recv(client->sockfd, buffer, MAX_BUFF, 0);
    buffer[receive] = 0;

    if(receive > 0){

      send_message(client->uid, "%s: %s", client->name, buffer);
    
    }else
    if(receive == 0){
      printf("%s has left the server.\n", client->name);
      fflush(stdout);
      send_message(client->uid, buffer);
      leave_flag = 1;
    }else{
      printf("failed to read\n");
    }

  }

  //clean up 
  remove_client(client->uid);
  free(client);
  pthread_detach(pthread_self());

  return NULL;
}// handling_client

void send_message(int sender, const char* buff, ...){

  pthread_mutex_lock(&mutex);

  va_list args;
  va_start(args, buff);
  char buffer[MAX_BUFF+1];
  vsprintf(buffer, buff, args);

  for(int i = 0; i < MAX_CLI; i++){
    if(clients[i] && (clients[i]->uid != sender)){
      if(send(clients[i]->sockfd, buffer, strlen(buffer), 0) < 0){
        perror("failed to send message\n");
        break;
      }

    }
  }

  pthread_mutex_unlock(&mutex);
}//send_message


