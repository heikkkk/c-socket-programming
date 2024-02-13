#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#define PORT 8080

void server_calculator(int connection_fd) {
	char buffer[256];
	int n;
	for(;;) {
	
		// print the text "skriv inn uttrykk" from the server 
		bzero(buffer, 256);
		printf("skriv inn uttrykk: \n");
		n = 0;
		while ((buffer[n++] = getchar()) != '\n')
			;

		// write the expression given by the user to the server 
		// TODO do some validation and exception handling
		write(connection_fd, buffer, sizeof(buffer));
		bzero(buffer, 256);

		// read the calculated value of the expresion from the server 
		read(connection_fd, buffer, sizeof(buffer));
		printf("=\n");
		printf("%s\n", buffer);

		if (strncmp(buffer, "exit", 4) == 0) {
			printf("exiting...\n");
			break;
		}
	}
}

int main() {

	
	// create a socket
	int network_socket;
	struct sockaddr_in server_address;
	
	network_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (network_socket == -1) {
		printf("socket creation failed..\n");
		exit(0);
	}
	else {
		printf("socket created..\n");
	}

	// specify an address for the socket
	bzero(&server_address, sizeof(server_address));

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);
	server_address.sin_addr.s_addr = inet_addr("127.0.0.1"); // sin_addr is a struct, accessing field s_addr to get the address of the server we are connecting to	
	
	// connect returns an integer: 0 = OK, -1 = ERROR
	if(connect(network_socket, 
		(struct sockaddr*)&server_address, sizeof(server_address)) != 0) {
		printf("connection with the server failed...");
		exit(0);

	}
	else {
		printf("connected to the server...\n");
	}
	 

	// recive data from the server 
	printf("-------------Calculator------------\n");	
	server_calculator(network_socket);

	// close the socket
	close(network_socket);
	
	return 0;
}

