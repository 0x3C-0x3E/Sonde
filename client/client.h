#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>

#define SERVER_PORT 8080
#define BUFFER_SIZE 1024


int start_client(const char * server_ip, int port);

void * revc_thread(void * arg);

void * send_thread(void * arg);


extern bool client_running;

extern SOCKET client_socket;

extern struct sockaddr_in server;

extern const char * SERVER_IP;

extern int PORT;


