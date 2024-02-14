#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <strings.h>

#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include "threadqueue.h"

#define PORT 8080
#define SERVER_BACKLOG 100
#define THREAD_POOL_SIZE 5

pthread_t thread_pool[THREAD_POOL_SIZE];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void * server_calculator(void* p_connection_fd);
int eval_expression(char* expression);
void * thread_function(void *arg);

int main() {

	//TODO PROPER CLOSING, REUSE SAME PORT WITHOUT KILLLING PROCESS FROM TERMINAL	

	// create the server socket 
	int server_socket, client_socket, length, option = 1;
	struct sockaddr_in server_address, client_address;

	// initialize thread pool 
	for (int i = 0; i < THREAD_POOL_SIZE; i++) {
		pthread_create(&thread_pool[i], NULL, thread_function, NULL);
	}
	
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket == -1) {
		perror("socket creation failed..\n");
		exit(EXIT_FAILURE);
	} else {
		printf("socket created..\n");
	}
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

	bzero(&server_address, sizeof(server_address));
	// define the server address
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(PORT);

	// bind the socket to the specified IP and port 
	if ((bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)))
			!= 0) {
		perror("socket bind failed\n");
		exit(EXIT_FAILURE);
	} else {
		printf("socket binded..\n");
	}

	// listen for connections. 5 specifies the total number of allowed connections at a time
	if (listen(server_socket, SERVER_BACKLOG) != 0) {
		perror("listen failds..\n");
		exit(EXIT_FAILURE);
	} else {  
		printf("server listening..\n");
	}

	while (1) {
			
		// define client socket 
		length = sizeof(client_address);
		client_socket = accept(server_socket, 
				(struct sockaddr*)&client_address, (socklen_t*)&length);
		if (client_socket < 0) {
			perror("server accept failed..\n");
			exit(EXIT_FAILURE);
		} else {
			printf("server accepted the client\n");
		}

		

		int *pclient = malloc(sizeof(int));
		*pclient = client_socket;
		pthread_mutex_lock(&mutex);
		enqueue(pclient);
		pthread_mutex_unlock(&mutex);
	}
	close(server_socket);
	return 0;
}

void * thread_function(void *arg) {
	while (1) {
		pthread_mutex_lock(&mutex);
		int *pclient = dequeue();
		pthread_mutex_unlock(&mutex);

		if (pclient != NULL) {
			server_calculator(pclient);
		}
	}
}

int eval_expression(char* expression) {
	printf("\nentred expression eval\n");

	size_t length = strlen(expression);
	int num1 = 0, num2 = 0;
	char operator;

	int i = 0;
	// get first number of the expression 
	while (expression[i] != '\0') {
		if (expression[i] == '+' || expression[i] == '-') {
			operator = expression[i];
			i++;
			break;
		}
		num1 = num1 * 10 + (expression[i] - '0');
		i++;
	}
	printf("num 1: %d\n", num1);

	printf("operator: %c\n", operator);
	printf("num2: %d\n", num2);
	// get second number of the expression 
	while (expression[i] != '\n') {
		num2 = num2 * 10 + (expression[i] - '0');
		printf("expression : %c\n", expression[i]);
		printf("num2: %d\n", num2);
		i++;
	}
	
	printf("exit expression eval\n");
	if (operator == '+') return num1 + num2;
	if (operator == '-') return num1 - num2;
	return 0;	
}

void * server_calculator(void* p_connection_fd) {
	int connection_fd = *((int*)p_connection_fd);
	free(p_connection_fd);

	char buffer[256];
	
	for(;;)
	{
		bzero(buffer, 256);

		// reads the expression from the client 
		read(connection_fd, buffer, sizeof(buffer));
		printf("uttrykk fra klient: %s", buffer);
		
		// calulates expression 
		int expression_value = eval_expression(buffer);
		printf("%s = %d", buffer, expression_value);
		
		bzero(buffer, 256);
		
		sprintf(buffer, "%d", expression_value);	
		// write expression and result to client 
		write(connection_fd, buffer, sizeof(buffer));

		if(strncmp("exit", buffer, 4) == 0) {
			printf("exiting...\n");
			break;
		}
	}
	return NULL;
}
