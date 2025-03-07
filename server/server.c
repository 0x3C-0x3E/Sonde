#include "server.h"

bool server_running = true;

WSADATA wsa_data;

SOCKET server_socket;

struct sockaddr_in server;


int num_clients = 0;

pthread_t threads[MAX_CLIENTS];

ThreadData thread_data[MAX_CLIENTS];

SOCKET clients[MAX_CLIENTS];


int init_wsa()
{
	if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
	{
		printf("Failed to initialize Winsock. Error: %d\n", WSAGetLastError());
		return 1;
	}

	return 0;
}

int init_server()
{

	// Create Socket

	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket == INVALID_SOCKET)
	{
		printf("Could not create socket. Error %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	// Configure server addr struc

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);

	// Bind to socket

	if (bind(server_socket, (struct sockaddr * ) & server,  sizeof(server)) == SOCKET_ERROR)
	{
		printf("Failed to bind to socket. Errror: %d\n", WSAGetLastError());
		closesocket(server_socket);
		WSACleanup();
		return 1;
	}

	return 0;
}

int start_server()
{
	if (init_wsa() == 1)
	{
		return 1;
	}

	if (init_server() == 1)
	{
		return 1;
	}

	// listen for incomming connections
	if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR)
	{
		printf("Failed to listen. Errror: %d\n", WSAGetLastError());
		closesocket(server_socket);
		WSACleanup();
		return 1;
	}

	printf("[SERVER] Server is listening on port %d\n", PORT);

	while (server_running)
	{
		struct sockaddr_in client;
		int client_size = sizeof(client);
		clients[num_clients] = accept(server_socket, (struct sockaddr * ) &client, &client_size);
		if (clients[num_clients] == INVALID_SOCKET)
		{
			printf("Accept failed. Error: %d\n", WSAGetLastError());
			continue;
		}

		char ip_address[64];

		get_client_ip_address(ip_address, sizeof(ip_address), &clients[num_clients], &client, &client_size);

		printf("[SERVER] New connection established with client: %s\n", ip_address);

		create_thread(&clients[num_clients], &client);

	}

	for (int i = 0; i < num_clients; i++)
	{
		pthread_join(threads[i], NULL);
	}

	closesocket(server_socket);
	WSACleanup();

	return 0;
}

void * handle_client(void * arg)
{
	ThreadData * data = (ThreadData * ) arg;


	char buffer[BUFFER_SIZE];
	int bytes_read;

	bool client_running = true;

	char ip_address[64];

	int thread_client_size = sizeof(*data->client);

	get_client_ip_address(ip_address, sizeof(ip_address), data->client_socket, data->client, &thread_client_size);

	const char * message = "checkconn";
	send(* data->client_socket, message, strlen(message), 0);

	while (client_running)
	{
		memset(buffer, 0, BUFFER_SIZE);
		bytes_read = recv(*data->client_socket, buffer, BUFFER_SIZE, 0);

		if (bytes_read <= 0) {
			printf("[WARNING] Client disconnected disgracefully\n");
			closesocket(*data->client_socket);
			break;
		}

		send_to_all(buffer, sizeof(buffer), data->client_socket);

		printf("[%s] %s\n", ip_address, buffer);

		if (strcmp(buffer, "checkconn_back") == 0)
		{
			printf("[INFO] Successfully completed ping roundtrip to client [%s]\n", ip_address);
		}
	}

	return NULL;
}


void create_thread(SOCKET * client_socket, struct sockaddr_in * client)
{
	thread_data[num_clients].thread_id = num_clients;
	thread_data[num_clients].client_socket = client_socket;
	thread_data[num_clients].client = client;


	int rc = pthread_create(&threads[num_clients], NULL, handle_client, &thread_data[num_clients]);

	if (rc) {
		printf("Error creating thread\n");
		return; 
	}

	num_clients ++;
}


void get_client_ip_address(char ip[], int size, SOCKET * client_socket, struct sockaddr_in * client, int * client_size)
{
	if (getpeername(*client_socket, (struct sockaddr * ) client, client_size) == SOCKET_ERROR)
	{
		printf("getpeername failed: %d\n", WSAGetLastError());
		return;
	}

	char ip_address [INET_ADDRSTRLEN];

	// Convert the IP address to a string
	if (inet_ntop(AF_INET, &client->sin_addr, ip_address, INET_ADDRSTRLEN) == NULL)
	{
		printf("inet_ntop failed\n");
		return;
	}
	snprintf(ip, size, ip_address);

}


void send_to_all(char message[], int size, SOCKET * sender_socket)
{
	for (int i = 0; i < num_clients; i++)
	{
		if (&clients[i] == sender_socket)
		{
			continue;
		}
		send(clients[i], message, size, 0);

	}
}