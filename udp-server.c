#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#define PORT 9002
#define BUFFER_SIZE 256

void * check(int sockfd, char* err_origin);

int main() {

    struct sockaddr_in server_address, client_address;
    bzero(&server_address, sizeof(server_address));
    bzero(&client_address, sizeof(client_address));
    int server_sockfd, cli_addr_len = sizeof(client_address), option = 1;
    
    char buffer[BUFFER_SIZE];

    server_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    check(server_sockfd, "socket created");

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(PORT);

    check(
        bind(server_sockfd, 
        (struct sockaddr*)&server_address, 
        sizeof(server_address)),
        "socket bind"
    );

    while(1) {
        int n;
        n = recvfrom(server_sockfd, buffer, BUFFER_SIZE, MSG_WAITALL, 
            (struct sockaddr*)&client_address, &cli_addr_len);
        
        buffer[n] = '\0';
        if (strcmp(buffer, "exit") == 0) {
            break;
        }

        int expr_val = eval_expression(buffer); 
        printf("Client : %s\n", buffer);
        printf("Server: %d\n", expr_val);
        
        sendto(server_sockfd, buffer, BUFFER_SIZE, MSG_CONFIRM,
            (stuct sockaddr*)&client_address, &cli_addr_len);
    }
    printf("closing server..\n");
    close(server_sockfd);
    return 0;
}

void * check(int sockfd, char* err_origin) {
    char err_msg = "error: ";
    strcat(err_msg, err_origin);
    if(sockfd < 0) {
        printf("%s", err_msg);
        exit(EXIT_SUCCESS);
        return NULL;
    }
    printf("%s", err_origin);
    return NULL;
}

int eval_expression(char* expression) {
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
		i++;
	}
	
	if (operator == '+') return num1 + num2;
	if (operator == '-') return num1 - num2;
	return 0;	
}