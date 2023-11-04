#include "server.h"

void check_err(int errn, const char* fmt){

  if(errn == -1){
    fprintf(stderr, "error: %s\n", fmt);
    exit(1);
  }

}// check()



void add_client(Client* cli){
  pthread_mutex_lock(&mutex);

  for(int i = 0; i < MAX_CLI; i++){
    if(!clients[i]){
      clients[i] = cli;
      break;
    }
  }
  cli_count++;

  pthread_mutex_unlock(&mutex);
}// add_client()

void remove_client(int cliuid){
  pthread_mutex_lock(&mutex);
  
  for(int i = 0; i < MAX_CLI; i++){
    if(clients[i] && (clients[i]->uid == cliuid)){
      clients[i] = NULL;
      break;
    }
  }
  cli_count--;
  pthread_mutex_unlock(&mutex);
}// remove_client()