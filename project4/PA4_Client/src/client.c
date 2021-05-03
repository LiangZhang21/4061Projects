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

// Array to store words length
int wordsLength[WORD_LENGTH_RANGE];
bool is_empty_array = 1;

FILE *logfp;
// Codes provided from pervious assignment
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
// To update the words length array
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
    if (argc != 6) {
        fprintf(stderr, "Usage: ./client <Folder Name> <# of Clients> <Server IP> <Server Port> -e\n");
        exit(1);
    }
    // initialized variables
    int i;
    // To store the path
    char folder_name[200];
    strcpy(folder_name, argv[1]);
    // To store the amount of clients
    int clients_num = atoi(argv[2]);
    if (clients_num <= 0 || clients_num > MAX_NUM_CLIENTS) {
        fprintf(stderr, "Invalid input: client number must be between 1 and 20\n");
        exit(1);
    }
    // To store the IP address
    char server_IP[200];
    strcpy(server_IP, argv[3]);
    // To store the port number
    short server_port = atoi(argv[4]);

    // Setting up socket address
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
            // Setting up a request structure
            int request_structure[REQUEST_MSG_SIZE];
            int client_ID = i + 1;
            int j;

            // Client ID
            request_structure[RQS_CLIENT_ID] = client_ID;
            request_structure[RQS_PERSISTENT_FLAG] = 1;
            //==============UPDATE_WSTAT============
            int sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (sockfd == -1) {
                perror("Failed to create socket");
                exit(1);
            }
            if (connect(sockfd, (struct sockaddr *)&address, sizeof(address)) == 0) {
                fprintf(logfp, "[%d] open connection\n", client_ID);
                // Get the words length stats
                int temp = client_ID;
                char fileName[500];
                sprintf(fileName, "%s/%d.txt", folder_name, client_ID);
                int count_update = 0;
                // Continues to open files and make request until the end of streams
                while (getWordsStats(fileName) == 0) {
                    temp += clients_num;
                    sprintf(fileName, "%s/%d.txt", folder_name, temp);
                    int read_buf[LONG_RESPONSE_MSG_SIZE];
                    // Skip if empty file
                    if (is_empty_array == 0) {
                        // Setting up the request structure
                        request_structure[RQS_RQS_CODE_NUM] = UPDATE_WSTAT;
                        for (j = RQS_DATA; j < RQS_PERSISTENT_FLAG; j++) {
                            request_structure[j] = wordsLength[j - 2];
                        }
                        if (write(sockfd, request_structure, sizeof(request_structure)) == -1) {
                            perror("Failed to write to the socket");
                            exit(1);
                        }
                        if (read(sockfd, read_buf, sizeof(read_buf)) == -1) {
                            perror("Failed to read from the socket");
                            exit(1);
                        }
                        if (read_buf[RSP_RSP_CODE_NUM] == RSP_NOK) {
                            fprintf(logfp, "[%d] close connection (failed execution)\n", client_ID);
                            exit(1);
                        }
                        // Reset the words counts after each request
                        for (j = 0; j < 20; j++) {
                            wordsLength[j] = 0;
                        }
                        count_update++;
                    }
                }
                fprintf(logfp, "[%d] UPDATE_WSTAT: %d\n", client_ID, count_update);

                //==========GET_MY_UPDATES=============
                //request structure set up
                request_structure[RQS_RQS_CODE_NUM] = GET_MY_UPDATES;
                memset(request_structure + RQS_DATA, 0, sizeof(int) * WORD_LENGTH_RANGE);
                if (write(sockfd, request_structure, sizeof(request_structure)) == -1) {
                    perror("Failed to write to the socket");
                    exit(1);
                }
                //Read back message from socket
                int read_buf2[LONG_RESPONSE_MSG_SIZE];
                if (read(sockfd, read_buf2, sizeof(read_buf2)) == -1) {
                    perror("Failed to read from the socket");
                    exit(1);
                }
                if (read_buf2[RSP_RSP_CODE_NUM] == RSP_NOK) {
                    fprintf(logfp, "[%d] close connection (failed execution)\n", client_ID);
                    exit(1);
                }
                fprintf(logfp, "[%d] GET_MY_UPDATES: %d %d\n", client_ID, read_buf2[RSP_RSP_CODE_NUM], read_buf2[RSP_DATA]);

                //==========GET_ALL_UPDATES===========
                //request structure set up
                request_structure[RQS_RQS_CODE_NUM] = GET_ALL_UPDATES;
                if (write(sockfd, request_structure, sizeof(request_structure)) == -1) {
                    perror("Failed to write to the socket");
                    exit(1);
                }
                //Read back message from socket
                int read_buf3[LONG_RESPONSE_MSG_SIZE];
                if (read(sockfd, read_buf3, sizeof(read_buf3)) == -1) {
                    perror("Failed to read from the socket");
                    exit(1);
                }
                if (read_buf3[RSP_RSP_CODE_NUM] == RSP_NOK) {
                    fprintf(logfp, "[%d] close connection (failed execution)\n", client_ID);
                    exit(1);
                }
                fprintf(logfp, "[%d] GET_ALL_UPDATES: %d %d\n", client_ID, read_buf3[RSP_RSP_CODE_NUM], read_buf3[RSP_DATA]);

                //===========GET_WSTAT================
                //request structure set up
                request_structure[RQS_RQS_CODE_NUM] = GET_WSTAT;
                request_structure[RQS_PERSISTENT_FLAG] = 0;
                if (write(sockfd, request_structure, sizeof(request_structure)) == -1) {
                    perror("Failed to write to the socket");
                    exit(1);
                }
                //Read back message from socket
                int read_buf4[LONG_RESPONSE_MSG_SIZE];
                if (read(sockfd, read_buf4, sizeof(read_buf4)) == -1) {
                    perror("Failed to read from the socket");
                    exit(1);
                }
                if (read_buf4[RSP_RSP_CODE_NUM] == RSP_NOK) {
                    fprintf(logfp, "[%d] close connection (failed execution)\n", client_ID);
                    exit(1);
                }
                fprintf(logfp, "[%d] GET_WSTAT: %d", client_ID, read_buf4[RSP_RSP_CODE_NUM]);
                for (j = 0; j < 20; j++) {
                    fprintf(logfp, " %d", read_buf4[j + RQS_DATA]);
                }
                fprintf(logfp, "\n");
                fprintf(logfp, "[%d] close connection (successful execution)\n", client_ID);
                if (close(sockfd)) {
                    perror("Failed to close the socket");
                    exit(1);
                }
            } else {
                perror("Sockfd connection failed!");
                exit(1);
            }
            break;
        } else if (client < 0) {
            perror("Failed to fork");
            exit(1);
        }
    }

    // wait for all client processes to terminate
    while (wait(NULL) > 0)
        ;

    // close log file
    fclose(logfp);

    return EXIT_SUCCESS;
}
