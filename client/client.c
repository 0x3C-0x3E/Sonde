#include "client.h"


int start_client(const char * server_ip, int port)
{
	int client_socket = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in address;
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(port);

	// Set address to your computer's local address
	inet_aton(server_ip, (struct in_addr *) &(address.sin_addr.s_addr));

	// Establish a connection to address on client_socket
	connect(client_socket, (struct sockaddr *) &address, sizeof(address));

	const char * message = "conn_est";

	send(client_socket, message, strlen(message), 0);

	// Close the connection
	close(client_socket);

	printf("%s", message);
	return 0;

}
