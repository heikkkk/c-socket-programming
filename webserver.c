#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>

#define PORT 9002
#define BACKLOG 100
#define BUFFER_SIZE 1024


void * parse_response(char* str, char* http_headers);
void remove_char(char* str, char c);

int main()
{
	char buffer[BUFFER_SIZE];
	
	struct sockaddr_in 
		server_address, 
		client_address;

	int server_socket, 
	    server_addrlen = sizeof(server_address), 
	    client_addrlen = sizeof(client_address);

	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket == -1) {
		perror("socket creation failed..\n");
		exit(EXIT_FAILURE);
	}
	printf("socket created..\n");
	
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(PORT);

	if (bind(server_socket, (struct sockaddr *)&server_address, server_addrlen) != 0) {
		perror("socket bind failed..\n");
	}
	printf("socket bound..\n");

	if (listen(server_socket, BACKLOG) != 0) {
		perror("listen failed..\n");
	}
	printf("server listening..\n");
	
	while (1) {
		int client_socket = accept(
				server_socket, 
				(struct sockaddr *)&server_address,
				(socklen_t *)&server_addrlen
				);
		if (client_socket < 0) {
			perror("server accept failed..\n");
			continue;
		}
		printf("connection accepted\n");

		// client address 
		int client_socket_name = getsockname(client_socket, 
				(struct sockaddr *)&client_address,
				(socklen_t *)&client_addrlen);

		int valread = read(client_socket, buffer, BUFFER_SIZE);
		if (valread < 0) {
			perror("error reading..\n");
			continue;
		}

		char server_response[BUFFER_SIZE];	
		bzero(server_response, BUFFER_SIZE);
		parse_response(server_response, buffer);

		int valwrite = write(client_socket, 
				server_response, strlen(server_response));
		if (valwrite < 0) {
			perror("error writing..\n");
			continue;
		}

		close(client_socket);
	}	

	return EXIT_SUCCESS;
}	

void remove_char(char* str, char c) {
	int j;
	for (int i = j = 0; i < strlen(str); i++) 
		if (str[i] != c) 
			str[j++] = str[i];
		

	str[j] = '\0';
}

void * parse_response(char* str, char* http_headers) {
	char substring_1[BUFFER_SIZE];
	bzero(substring_1, BUFFER_SIZE);
	strcat(substring_1, "HTTP/1.0 200 OK\r\n"
		"Content-type: text/html; charset=utf-8\r\n\r\n"
		"<HTML><BODY>"
		"<H1>Hilsen. Du har koblet deg opp til min enkle web-tjener</H1>"
		"Header fra klient er:"
		"<UL>");
	
	char substring_2[] =
		"</UL>"
		"</BODY></HTML>\r\n"
		;
	
	char return_str[BUFFER_SIZE];
	char str_temp[256];
	int k;
	for (int i = k = 0; http_headers[i] != '\0'; i++) {
		str_temp[k] = http_headers[i];
		k++;	

		if (http_headers[i] == '\n' && http_headers[i+1] != '\r') {
			remove_char(str_temp, '\r');
			remove_char(str_temp, '\n');
			strcat(return_str, "<LI>");
			strcat(return_str, str_temp);
			strcat(return_str, "</LI>");
			printf("%s", str_temp);
			bzero(str_temp, 256);
			k = 0;
		}	

	}
	strcat(substring_1, return_str);
	strcat(substring_1, substring_2);
	strcat(str, substring_1);
	return NULL;
}
