#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdarg.h>
#include <signal.h>

#define MAX_CLI 10 // amount of clients that can connect to the server
#define MAX_BUFF 1024 

typedef struct CLIENT{
  int sockfd;
  int uid;
  int tid;
  char name[16];

}Client;

extern void check_err(int errn, const char* fmt); // for checking error of a function
extern void add_client(Client* cli); // add client to the list 
extern void remove_client(int cliuid); // remove client fron the list
extern void send_message(int sender, const char* buff, ...); // send any message to all clients
extern void* handling_client(void* cli); 

extern int cli_count; 
extern Client* clients[MAX_CLI];
extern pthread_mutex_t mutex;



#endif