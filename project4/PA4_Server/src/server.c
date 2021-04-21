#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <zconf.h>
#include <pthread.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include "../include/protocol.h"

#define NUM_ARGS 2
#define MAX_CONNECTIONS 20
int words_length[20];
struct thread_args {
		int thread_count;
		int clientfd;
		int client_port;	
		char* client_ip;
};

struct thread_args thread_arg1;

void * socket_thread(void *arg) {
	struct thread_args *new_arg = (struct thread_args*)arg;
	int new_socket = new_arg -> clientfd;
	int client_ID = new_arg -> thread_count;
	char* client_ip = new_arg -> client_ip;
	int client_port = new_arg -> client_port;
	int recv_buf[REQUEST_MSG_SIZE];
	char send_buf[1000];
	
	if(read(new_socket, recv_buf, sizeof(recv_buf)) > 0) {
		//printf("recv_buf: %d\n", recv_buf[1]);
		if (recv_buf[0] == UPDATE_WSTAT ) {
			
		} else if (recv_buf[0] == GET_MY_UPDATES) {
			
		} else if (recv_buf[0] == GET_ALL_UPDATES) {
			
		} else if (recv_buf[0] == GET_WSTAT) {
			printf("[%d] GET_WSTAT\n", client_ID);
			strcpy(send_buf, "Hello Word");
			write(new_socket, send_buf, 1000);
		}
		
	}
	printf("close connection from %s:%d\n", client_ip, client_port);
	close(new_socket);	
	return NULL;	
}

int main(int argc, char *argv[]) {

    // process input arguments
	if (argc != NUM_ARGS) {
		printf("Wrong number of args, expected %d\n", NUM_ARGS);
		exit(1);
	}
	
    // socket create and verification 
	int sock = socket(AF_INET , SOCK_STREAM , 0);

    // assign IP, PORT 
	short server_port = atoi(argv[1]);
	int IP = INADDR_ANY;
	
	struct sockaddr_in servAddress;
	servAddress.sin_family = AF_INET;
	servAddress.sin_port = htons(server_port);
	servAddress.sin_addr.s_addr = htonl(IP);
	
    // Binding newly created socket to given IP
	if( bind(sock, (struct sockaddr *) &servAddress, sizeof(servAddress)) < 0){
		fprintf(stderr, "Failed to bind socket thread.\n");	
	}
	
    // Now server is ready to listen
    listen(sock, MAX_CONNECTIONS);
	printf("server is listening\n");
	
	// Creating a struct for thread args
	
	thread_arg1.thread_count = 0;
	
    while (1) {

        // Accept the data packet from client
        struct sockaddr_in clientAddress;
		pthread_t new_thread[MAX_CONNECTIONS];
		socklen_t size = sizeof(struct sockaddr_in);
		
		if( (thread_arg1.clientfd = accept(sock, (struct sockaddr*) &clientAddress, &size)) < 0 ) {
			fprintf(stderr, "Failed to accpet socket.\n");
		}
		thread_arg1.client_ip = inet_ntoa(clientAddress.sin_addr);
		thread_arg1.client_port = clientAddress.sin_port;
		//printf("%s\n", clientIpAddr);
		printf("open connection from %s:%d\n", thread_arg1.client_ip, thread_arg1.client_port);
		
		if( pthread_create(&new_thread[thread_arg1.thread_count++], NULL, socket_thread, &thread_arg1) != 0 ) {
			fprintf(stderr, "Failed to create socket thread.\n");
		}
		
		if(thread_arg1.thread_count >= MAX_CONNECTIONS) {
			int i;
			for(i = 0; i < 50; i++) {
				pthread_join(new_thread[i], NULL);
			}
			thread_arg1.thread_count = 0;
		}
		
    }
    exit(EXIT_SUCCESS);
} 
