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
#include <stdbool.h>

#define NUM_ARGS 5

//Array to store words length
int wordsLength[20];
bool is_empty_array = 1;

FILE *logfp;

ssize_t getLineFromFile(FILE *fp, char *line, size_t len) {
    memset(line, '\0', len);
    return getline(&line, &len, fp);
}

void createLogFile(void) {
    pid_t p = fork();
    if (p == 0)
        execl("/bin/rm", "rm", "-rf", "log", NULL);

    wait(NULL);
    mkdir("log", ACCESSPERMS);
    logfp = fopen("log/log_client.txt", "w");
}

int getWordsStats(char* path, int clientID){
	char fileName[500];
	sprintf(fileName, "%s/%d.txt", path, clientID);
	FILE *fp = fopen(fileName, "r");
	if (fp == NULL) return 1;
	char readbuf[1000];
	while ( getLineFromFile(fp, readbuf, sizeof(readbuf)) > 0) {
		char *token = strtok(readbuf, "\n");
		token = strtok(token, " ");
		while (token != NULL) {
			wordsLength[strlen(token) - 1]++;
			token = strtok(NULL, " ");
		}
	}
	is_empty_array = 0;
	
	return 0;
}

int main(int argc, char *argv[]) {

    // process input arguments
	if (argc != NUM_ARGS) {
		printf("Wrong number of args, expected %d", NUM_ARGS);
		exit(1);
	}
	int i;
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
    
    int request_method = 3;
    
	for(i = 0; i < clients_num; i++){
		if( (client = fork()) == 0 ) {
			int sockfd = socket(AF_INET , SOCK_STREAM , 0);
			if (connect(sockfd, (struct sockaddr *) &address, sizeof(address)) == 0) {
				printf("[%d] open connection\n", i+1);	
				int request_structure[23];
				//Client ID
				request_structure[1] = i+1;
				//UPDATE_WSTAT
				int j;
				if (request_method == UPDATE_WSTAT) {
					//Get the words length stats		
					getWordsStats(folder_name, i+1);
					if(is_empty_array == 0) {
						request_structure[0] = 1;
						
						for(j = 2; j < 23; j++) {
							request_structure[j] = wordsLength[j-2];
						}
						write(sockfd, request_structure, sizeof(request_structure));
					}
				} 
				//GET_MY_UPDATES
				else if (request_method == GET_MY_UPDATES) {
					request_structure[0] = 2;
					write(sockfd, request_structure, sizeof(request_structure));
				
					int read_buf[LONG_RESPONSE_MSG_SIZE];
					read(sockfd, read_buf, sizeof(read_buf));
					printf("[%d] GET_MY_UPDATES: %d %d\n", i, 1, read_buf[i]);
				} 
				//GET_ALL_UPDATES
				else if (request_method == GET_ALL_UPDATES) {
					request_structure[0] = 3;
					write(sockfd, request_structure, sizeof(request_structure));
				
					int read_buf[LONG_RESPONSE_MSG_SIZE];
					read(sockfd, read_buf, sizeof(read_buf));
					printf("[%d] GET_ALL_UPDATES: %d ", i, 1);
					for(j = 0; j < 20; j++){
						printf("%d", read_buf[i]);
					}
					printf("\n");
				} 
				//GET_WSTAT
				else if (request_method == GET_WSTAT) {		
					printf("[%d] GET_WSTAT: %d <20 numbers>\n", i+1, 1);
					request_structure[0] = 4;
					write(sockfd, request_structure, sizeof(request_structure));
					
					int read_buf[LONG_RESPONSE_MSG_SIZE];
					read(sockfd, read_buf, sizeof(read_buf));
					for(j = 0; j < 20; j++){
						printf("word length: %d, counts: %d\n", j+1, read_buf[j]);
					}
				}
				
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
