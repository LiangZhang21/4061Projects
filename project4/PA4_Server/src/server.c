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
//setting up mutex lock
pthread_mutex_t mutex;
//Arrays for stats
int words_length[20];
int request_count[20];

struct thread_args {
		int thread_count;
		int clientfd;
		int client_port;	
		char* client_ip;
};

struct thread_args thread_arg;

void update_wstat(int* request_structure) {
	int i;
	for(i = 0; i < 20; i++) {	
		words_length[i] += request_structure[i+2];
		printf("index:%d, value:%d\n", i, words_length[i]);
	}
}

void * socket_thread(void *arg) {
	struct thread_args *new_arg = (struct thread_args*)arg;
	int new_socket = new_arg -> clientfd;
	int client_ID = new_arg -> thread_count;
	char* client_ip = new_arg -> client_ip;
	int client_port = new_arg -> client_port;
	int recv_buf[REQUEST_MSG_SIZE];
	
	if(read(new_socket, recv_buf, sizeof(recv_buf)) > 0) {
		printf("recv_buf: %d\n", recv_buf[1]);
		if (recv_buf[0] == UPDATE_WSTAT ) {
			printf("[%d] UPDATE_WSTAT\n", client_ID);
			if (pthread_mutex_lock(&mutex)) {
				fprintf(stderr, "failed to lock update_wstat");
				pthread_exit((int *) 1);
			}
			update_wstat(recv_buf);
			request_count[client_ID-1]++;
			if (pthread_mutex_unlock(&mutex)) {
				fprintf(stderr, "failed to unlock update_wstat");
				pthread_exit((int *) 1);
			}	
		} else if (recv_buf[0] == GET_MY_UPDATES) {
			printf("[%d] GET_MY_UPDATES\n", client_ID);
			write(new_socket, request_count, sizeof(request_count));
		} else if (recv_buf[0] == GET_ALL_UPDATES) {
			printf("[%d] GET_ALL_UPDATES\n", client_ID);
			write(new_socket, request_count, sizeof(request_count));
		} else if (recv_buf[0] == GET_WSTAT) {
			printf("[%d] GET_WSTAT\n", client_ID);
			write(new_socket, words_length, sizeof(words_length));
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
	//init mutex lock
	if (pthread_mutex_init(&mutex, NULL)) {
		fprintf(stderr, "Failed to init mutex lock\n");
		return 1;
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
	
	thread_arg.thread_count = 0;
	
    while (1) {

        // Accept the data packet from client
        struct sockaddr_in clientAddress;
		pthread_t new_thread[MAX_CONNECTIONS];
		socklen_t size = sizeof(struct sockaddr_in);
		
		if( (thread_arg.clientfd = accept(sock, (struct sockaddr*) &clientAddress, &size)) < 0 ) {
			fprintf(stderr, "Failed to accpet socket.\n");
		}
		thread_arg.client_ip = inet_ntoa(clientAddress.sin_addr);
		thread_arg.client_port = clientAddress.sin_port;
		//printf("%s\n", clientIpAddr);
		printf("open connection from %s:%d\n", thread_arg.client_ip, thread_arg.client_port);
		
		if( pthread_create(&new_thread[thread_arg.thread_count++], NULL, socket_thread, &thread_arg) != 0 ) {
			fprintf(stderr, "Failed to create socket thread.\n");
		}
		
		if(thread_arg.thread_count >= MAX_CONNECTIONS) {
			int i;
			for(i = 0; i < 50; i++) {
				pthread_join(new_thread[i], NULL);
			}
			thread_arg.thread_count = 0;
		}
		
    }
    exit(EXIT_SUCCESS);
} 
