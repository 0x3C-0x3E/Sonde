#include "client.h"
#include <string.h>


bool client_running;

SOCKET client_socket;

struct sockaddr_in server;

const char * SERVER_IP;

int PORT;




char send_buffer[BUFFER_SIZE];

buffer_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t buffer_cond = PTHREAD_COND_INITIALIZER;

int data_available = 0;




int start_client(const char * server_ip, int port)
{
	client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (client_socket == INVALID_SOCKET)
	{
		printf("Could not create socket.\n");
		return 1;
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(port);

	PORT = port;
	SERVER_IP = server_ip;

	// convert server ip addr from string to binary form
	if (inet_pton(AF_INET, SERVER_IP, &server.sin_addr.s_addr) <= 0)
	{
		printf("Invalid server IP address\n");
		close(client_socket);
		return 1;
	}

	// Set address to your computer's local address
	if (connect(client_socket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Failed to connect to server\n");
		close(client_socket);
		return 1;
	}

	printf("[INFO] Connected to server %s:%d\n", SERVER_IP, PORT);


	while (true)
	{
		pthread_t send_t;
		pthread_t recv_t;


		if (pthread_create(&send_t, NULL, send_thread, NULL))
		{
			printf("Error creating thread\n");
			close(client_socket);
			return 1;
		}


		if (pthread_create(&recv_t, NULL, revc_thread, NULL))
		{
			printf("Error creating thread\n");
			close(client_socket);
			return 1; 
		}

		pthread_join(send_t, NULL);
		pthread_join(recv_t, NULL);
	}

	close(client_socket);

	return 0;

}


void * revc_thread(void * arg)
{
	while (true)
	{
		char buffer[BUFFER_SIZE];

		int bytes_recv = recv(client_socket, buffer, BUFFER_SIZE, 0);
		if (bytes_recv > 0)
		{
			buffer[bytes_recv] = '\0'; 
			printf("%s\n", buffer);


			// ## handle req ##

			pthread_mutex_lock(&buffer_mutex);

			snprintf(send_buffer, BUFFER_SIZE, "checkconn_back");

			data_available = 1;


			pthread_cond_signal(&buffer_cond);
			pthread_mutex_unlock(&buffer_mutex);

		}
		else
		{
			printf("Server disconnected.\n");
		}
	}
	return NULL;
}

void * send_thread(void * arg)
{
	while (true)
	{
		pthread_mutex_lock(&buffer_mutex);

		// Wait until data is available
		while (!data_available) {
		pthread_cond_wait(&buffer_cond, &buffer_mutex);
		}

		send(client_socket, send_buffer, strlen(send_buffer), 0);

		data_available = 0; // Reset flag

		pthread_mutex_unlock(&buffer_mutex);
	}

	return NULL;
}
