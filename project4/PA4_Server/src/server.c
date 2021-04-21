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

void * socket_thread(void *arg) {
	int new_socket = *((int *)arg);
	int recv_buf[REQUEST_MSG_SIZE];
	char send_buf[1000];
	printf("inside socket_thread function\n");
	if(read(new_socket, recv_buf, sizeof(recv_buf)) > 0) {
		printf("recv_buf: %d\n", recv_buf[1]);
		if (recv_buf[0] == UPDATE_WSTAT ) {
			
		} else if (recv_buf[0] == GET_MY_UPDATES) {
			
		} else if (recv_buf[0] == GET_ALL_UPDATES) {
			
		} else if (recv_buf[0] == GET_WSTAT) {
			strcpy(send_buf, "Hello Word\n");
			write(new_socket, send_buf, 1000);
		}
		
	}
	
	close(new_socket);	
	return NULL;	
}

int main(int argc, char *argv[]) {

    // process input arguments
	if (argc != NUM_ARGS) {
		printf("Wrong number of args, expected %d\n", NUM_ARGS);
		exit(1);
	}
	
	printf("Setting up socket...\n");
    // socket create and verification 
	int sock = socket(AF_INET , SOCK_STREAM , 0);

    // assign IP, PORT 
	short server_port = atoi(argv[1]);
	int IP = INADDR_ANY;
	
	printf("Setting up sock address...\n");	
	struct sockaddr_in servAddress;
	servAddress.sin_family = AF_INET;
	servAddress.sin_port = htons(server_port);
	servAddress.sin_addr.s_addr = htonl(IP);
	
	printf("Binding socket...\n");
    // Binding newly created socket to given IP
	if( bind(sock, (struct sockaddr *) &servAddress, sizeof(servAddress)) < 0){
		fprintf(stderr, "Failed to bind socket thread.\n");	
	}
	
    // Now server is ready to listen
    listen(sock, MAX_CONNECTIONS);
	printf("Listening on port %d\n", server_port);
	
    while (1) {

        // Accept the data packet from client
        struct sockaddr_in clientAddress;
		pthread_t new_thread[MAX_CONNECTIONS];
		int thread_count = 0;
		socklen_t size = sizeof(struct sockaddr_in);
		int clientfd ;
		if( (clientfd = accept(sock, (struct sockaddr*) &clientAddress, &size)) < 0 ) {
			fprintf(stderr, "Failed to accpet socket.\n");
		}
		printf("client1 connected\n");
		
		if( pthread_create(&new_thread[thread_count++], NULL, socket_thread, &clientfd) != 0 ) {
			fprintf(stderr, "Failed to create socket thread.\n");
		}
		
		if(thread_count >= MAX_CONNECTIONS) {
			int i;
			for(i = 0; i < 50; i++) {
				pthread_join(new_thread[i], NULL);
			}
			thread_count = 0;
		}
		
    }
    exit(EXIT_SUCCESS);
} 
