#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <zconf.h>

#include "../include/protocol.h"

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
    mkdir("log", 0777);
    logfp = fopen("log/log_client.txt", "w");
}

int getWordsStats(char *path) {
    FILE *fp = fopen(path, "r");
    if (fp == NULL) return 1;
    char readbuf[1000];
    while (getLineFromFile(fp, readbuf, sizeof(readbuf)) > 0) {
        char *token = strtok(readbuf, "\n");
        token = strtok(token, " ");
        while (token != NULL) {
            wordsLength[strlen(token) - 1]++;
            token = strtok(NULL, " ");
        }
    }
    is_empty_array = 0;
	fclose(fp);
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

    for (i = 0; i < clients_num; i++) {
        if ((client = fork()) == 0) {
            int request_structure[23];
            int client_ID = i + 1;
            int j;

            //Client ID
            request_structure[1] = i + 1;
            //Socket for UPDATE_WSTAT
            int sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (connect(sockfd, (struct sockaddr *)&address, sizeof(address)) == 0) {
                fprintf(logfp, "[%d] open connection\n", client_ID);
                //Get the words length stats
                int temp = client_ID;
                char fileName[500];
    			sprintf(fileName, "%s/%d.txt", folder_name, client_ID);
                int count_update = 0;
                while (getWordsStats(fileName) == 0) {
                	temp += clients_num;
                	sprintf(fileName, "%s/%d.txt", folder_name, temp);
                	//printf("filename: %s\n", fileName);
                	int read_buf[LONG_RESPONSE_MSG_SIZE];
       	  		    if (is_empty_array == 0) {
                    	request_structure[0] = 1;
                    	request_structure[1] = client_ID;
                    	for (j = 2; j < 23; j++) {
                        	request_structure[j] = wordsLength[j - 2];
                    	}
                    	request_structure[22] = 1;      	
                    	write(sockfd, request_structure, sizeof(request_structure));       
               	 		read(sockfd, read_buf, sizeof(read_buf));           	 	   
                		for (j = 0; j < 20; j++) {
                			wordsLength[j] = 0;
                		}
                		count_update++;
                	}    	
                }
                fprintf(logfp, "[%d] UPDATE_WSTAT: %d\n", client_ID, count_update);
               	
               	printf("socket2\n");
				//socket 2
				
				fprintf(logfp, "[%d] open connection\n", client_ID);
                //request structure set up
                request_structure[0] = 2;
                request_structure[1] = client_ID;
                write(sockfd, request_structure, sizeof(request_structure));
                //Read back message from socket
                int read_buf2[LONG_RESPONSE_MSG_SIZE];
                read(sockfd, read_buf2, sizeof(read_buf2));
                fprintf(logfp, "[%d] GET_MY_UPDATES: %d %d\n", client_ID, read_buf2[1], read_buf2[2]);
                fprintf(logfp, "[%d] close connection (successful execution)\n", client_ID);
				
				printf("socket3\n");
				//socket 3
				fprintf(logfp, "[%d] open connection\n", client_ID);
                //request structure set up
                request_structure[0] = 3;
                request_structure[1] = client_ID;
                write(sockfd, request_structure, sizeof(request_structure));
                //Read back message from socket
                int read_buf3[LONG_RESPONSE_MSG_SIZE];
                read(sockfd, read_buf3, sizeof(read_buf3));

                fprintf(logfp, "[%d] GET_ALL_UPDATES: %d %d\n", client_ID, read_buf3[1], read_buf3[2]);
                fprintf(logfp, "[%d] close connection (successful execution)\n", client_ID);
                
                printf("socket4\n");
                //socket 4
                fprintf(logfp, "[%d] open connection\n", client_ID);
                //request structure set up
                request_structure[0] = 4;
                request_structure[1] = client_ID;
                request_structure[22] = 0;
                write(sockfd, request_structure, sizeof(request_structure));
                //Read back message from socket
                int read_buf4[LONG_RESPONSE_MSG_SIZE];
                read(sockfd, read_buf4, sizeof(read_buf4));

                fprintf(logfp, "[%d] GET_WSTAT: %d", client_ID, read_buf4[1]);
                for (j = 0; j < 20; j++) {
                    fprintf(logfp, " %d", read_buf4[j + 2]);
                }
                fprintf(logfp, "\n");
                fprintf(logfp, "[%d] close connection (successful execution)\n", client_ID);
                
                 close(sockfd);
                 
            } else {
                perror("Sockfd1 connection failed!");
            }

            //Socket for GET_MY_UPDATES
//            int sockfd2 = socket(AF_INET, SOCK_STREAM, 0);
//            if (connect(sockfd2, (struct sockaddr *)&address, sizeof(address)) == 0) {
//                
//            } else {
//                perror("Sockfd2 connection failed!");
//            }
//
            //Socket for GET_ALL_UPDATES
//            int sockfd3 = socket(AF_INET, SOCK_STREAM, 0);
//            if (connect(sockfd3, (struct sockaddr *)&address, sizeof(address)) == 0) {
//                
//            } else {
//                perror("Sockfd3 connection failed!");
//            }

            //Socket for GET_WSTAT
//            int sockfd4 = socket(AF_INET, SOCK_STREAM, 0);
//            if (connect(sockfd4, (struct sockaddr *)&address, sizeof(address)) == 0) {
//                
//            } else {
//                perror("Sockfd4 connection failed!");
//            }
			
       break;
   }
}

    // wait for all client processes to terminate
    while (wait(NULL) > 0)
        ;

    // close log file
    fclose(logfp);

    return EXIT_SUCCESS;
}
