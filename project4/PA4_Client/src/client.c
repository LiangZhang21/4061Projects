#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <zconf.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/wait.h>
#include "../include/protocol.h"
#define NUM_ARGS 5

FILE *logfp;

void createLogFile(void) {
    pid_t p = fork();
    if (p == 0)
        execl("/bin/rm", "rm", "-rf", "log", NULL);

    wait(NULL);
    mkdir("log", ACCESSPERMS);
    logfp = fopen("log/log_client.txt", "w");
}

int main(int argc, char *argv[]) {

    // process input arguments
	if (argc != NUM_ARGS) {
		printf("Wrong number of args, expected %d", NUM_ARGS);
		exit(1);
	}
	char folder_name[200];
	strcpy(folder_name, argv[1]);
	int clients_num = atoi(argv[2]);
	char server_IP[200];
	strcpy(server_IP, argv[3]);
	short server_port = atoi(argv[4]);
	
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(server_port);
	address.sin_addr.s_addr = inet_addr(server_IP);

    // create log file
    createLogFile();

    // spawn client processes
    pid_t client;
    
    int i;
	for(i = 0; i < clients_num; i++){
		if( (client = fork()) == 0 ) {
			int sockfd = socket(AF_INET , SOCK_STREAM , 0);
			if (connect(sockfd, (struct sockaddr *) &address, sizeof(address)) == 0) {
				printf("[%d] open connection\n", i+1);
				
				//GET_WSTAT
				int test[2] = {4,64};
				printf("[%d] GET_WSTAT: %d <20 numbers>\n", i+1, test[0]);
				write(sockfd, test, sizeof(test));
				
				char read_buf[200];
				read(sockfd, read_buf, 200);
				printf("read from server: %s\n", read_buf);

				close(sockfd); 
				printf("[%d] close connection (successful execution)\n", i+1);
				
				
			} else {

				perror("Connection failed!");
			}	
			
			break;		
		} 
	}

    // wait for all client processes to terminate
	while(wait(NULL) > 0);

    // close log file
    fclose(logfp);

    return EXIT_SUCCESS;

}
