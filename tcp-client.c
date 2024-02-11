#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

int main() {

	// create a socket
	int network_socket;
	network_socket = socket(AF_INET, SOCK_STREAM, 0);

	// specify an address for the socket
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9002);
	server_address.sin_addr.s_addr = INADDR_ANY; // sin_addr is a struct, accessing field s_addr to get the address of the server we are connecting to	
	
	// connect returns an integer: 0 = OK, -1 = ERROR
	int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
	// check for error with the connection 
	if (connection_status == -1) {
		printf("There was an error making a connection to the remote server \n\n");
	}

	// recive data from the server 
	char server_response[256];
	// put the data we recive from the socket in server_response. optional flags param is 0, left unused.
	recv(network_socket, &server_response, sizeof(server_response), 0);

	//print out the server's response
	printf("The server sent the data: %s\n", server_response);

	// close the socket
	close(network_socket);

	return 0;
}

