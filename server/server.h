#pragma once
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>


#include <pthread.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

typedef struct
{
	int thread_id;
	SOCKET * client_socket;
	struct sockaddr_in * client;

} ThreadData;

int init_wsa();

int init_server();

int start_server();

void create_thread(SOCKET * client_socket, struct sockaddr_in * client);

void * handle_client(void * arg);

void close_socket(int server);

void cleanup();


void get_client_ip_address(char ip[], int size, SOCKET * client_socket, struct sockaddr_in * client, int * client_size);

void send_to_all(char message[], int size, SOCKET * sender_socket);


extern bool server_running;


extern WSADATA wsa_data;

extern SOCKET server_socket;

extern struct sockaddr_in server;


const int client_size = sizeof(struct sockaddr_in);


extern int num_clients;

extern pthread_t threads[MAX_CLIENTS];

extern ThreadData thread_data[MAX_CLIENTS];

extern SOCKET clients[MAX_CLIENTS];
