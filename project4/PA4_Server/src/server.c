#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <zconf.h>

#include "../include/protocol.h"

#define MAX_CONNECTIONS 20
// setting up mutex lock
pthread_mutex_t mutex;
//Arrays for stats
int words_length[WORD_LENGTH_RANGE];
int request_count[WORD_LENGTH_RANGE];
int empty_data[LONG_RESPONSE_MSG_SIZE];

// Creating a struct for thread arguments
struct thread_args {
    int clientfd;
    int client_port;
    char *client_ip;
};
// To update the array
void update_wstat(int *request_structure) {
    int i;
    for (i = 0; i < WORD_LENGTH_RANGE; i++) {
        words_length[i] += request_structure[i + RQS_DATA];
    }
}
// To get the total count of request
int request_count_sum() {
    int i, sum = 0;
    if (pthread_mutex_lock(&mutex)) {
        fprintf(stderr, "failed to lock GET_ALL_UPDATES");
        pthread_exit((int *)1);
    }
    for (i = 0; i < WORD_LENGTH_RANGE; i++) {
        sum += request_count[i];
    }
    if (pthread_mutex_unlock(&mutex)) {
        fprintf(stderr, "failed to unlock GET_ALL_UPDATES");
        pthread_exit((int *)1);
    }
    return sum;
}
// To deal with request
void *socket_thread(void *arg) {
    // Setting up the struct args
    struct thread_args *new_arg = (struct thread_args *)arg;
    int new_socket = new_arg->clientfd;
    char *client_ip = new_arg->client_ip;
    int client_port = new_arg->client_port;
    int recv_buf[REQUEST_MSG_SIZE];
    int read_size;
    while (1) {
        // Reading in the data from socket
        if ((read_size = read(new_socket, recv_buf, sizeof(recv_buf))) > 0) {
            // Setting up variables
            int client_ID = recv_buf[RQS_CLIENT_ID];
            // Validating the data
            if (client_ID > 0 && client_ID < 21) {
                //===============UPDATE_WSTAT================
                if (recv_buf[RQS_RQS_CODE_NUM] == UPDATE_WSTAT) {
                    printf("[%d] UPDATE_WSTAT\n", client_ID);
                    //locking up the critical section
                    if (pthread_mutex_lock(&mutex)) {
                        fprintf(stderr, "failed to lock update_wstat");
                        close(new_socket);
                        free(arg);
                        pthread_exit((int *)1);
                    }
                    update_wstat(recv_buf);
                    request_count[client_ID - 1]++;
                    //unlocking the critical section
                    if (pthread_mutex_unlock(&mutex)) {
                        fprintf(stderr, "failed to unlock update_wstat");
                        close(new_socket);
                        free(arg);
                        pthread_exit((int *)1);
                    }
                    //Setting up the response
                    int update_log[RESPONSE_MSG_SIZE];
                    update_log[RSP_RQS_CODE_NUM] = UPDATE_WSTAT;
                    update_log[RSP_RSP_CODE_NUM] = RSP_OK;
                    update_log[RSP_DATA] = client_ID;
                    //Sending back the response
                    if (write(new_socket, update_log, sizeof(update_log)) == -1) {
                        fprintf(stderr, "Failed to write to the socket\n");
                        close(new_socket);
                        free(arg);
                        pthread_exit((int *)1);
                    }
                } else if (recv_buf[RQS_RQS_CODE_NUM] == GET_MY_UPDATES) {
                    //============GET_MY_UPDATES==============
                    printf("[%d] GET_MY_UPDATES\n", client_ID);
                    //locking up the critical section
                    if (pthread_mutex_lock(&mutex)) {
                        fprintf(stderr, "failed to lock GET_MY_UPDATES");
                        close(new_socket);
                        free(arg);
                        pthread_exit((int *)1);
                    }
                    //setting up the response
                    int count = request_count[client_ID - 1];
                    int get_my_updates[RESPONSE_MSG_SIZE] = {GET_MY_UPDATES, RSP_OK, count};
                    //Sending the response back to client
                    if (write(new_socket, get_my_updates, sizeof(get_my_updates)) == -1) {
                        fprintf(stderr, "Failed to write to the socket\n");
                        close(new_socket);
                        free(arg);
                        pthread_exit((int *)1);
                    }
                    //unlocking the critical section
                    if (pthread_mutex_unlock(&mutex)) {
                        fprintf(stderr, "failed to unlock GET_MY_UPDATES");
                        close(new_socket);
                        free(arg);
                        pthread_exit((int *)1);
                    }
                } else if (recv_buf[RQS_RQS_CODE_NUM] == GET_ALL_UPDATES) {
                    //==============GET_ALL_UPDATES=============
                    printf("[%d] GET_ALL_UPDATES\n", client_ID);
                    int count_sum = request_count_sum();
                    //Setting up the reponse message and send it back to client
                    int get_all_data[RESPONSE_MSG_SIZE] = {GET_ALL_UPDATES, RSP_OK, count_sum};
                    if (write(new_socket, get_all_data, sizeof(get_all_data)) == -1) {
                        fprintf(stderr, "Failed to write to the socket\n");
                        close(new_socket);
                        free(arg);
                        pthread_exit((int *)1);
                    }
                } else if (recv_buf[RQS_RQS_CODE_NUM] == GET_WSTAT) {
                    //==============GET_WSTAT====================
                    printf("[%d] GET_WSTAT\n", client_ID);
                    //Setting up the response structure
                    int get_wstat_reponse[LONG_RESPONSE_MSG_SIZE];
                    get_wstat_reponse[RSP_RQS_CODE_NUM] = GET_WSTAT;
                    get_wstat_reponse[RSP_RSP_CODE_NUM] = RSP_OK;
                    int z;
                    //Locking up the critical section
                    if (pthread_mutex_lock(&mutex)) {
                        fprintf(stderr, "failed to lock GET_MY_UPDATES");
                        close(new_socket);
                        free(arg);
                        pthread_exit((int *)1);
                    }
                    //Extracting the data to response message
                    for (z = 0; z < WORD_LENGTH_RANGE; z++) {
                        get_wstat_reponse[z + RSP_DATA] = words_length[z];
                    }
                    //Unlocking the critical section
                    if (pthread_mutex_unlock(&mutex)) {
                        fprintf(stderr, "failed to unlock GET_MY_UPDATES");
                        close(new_socket);
                        free(arg);
                        pthread_exit((int *)1);
                    }
                    //Sending the message back to client
                    if (write(new_socket, get_wstat_reponse, sizeof(get_wstat_reponse)) == -1) {
                        fprintf(stderr, "Failed to write to the socket\n");
                        close(new_socket);
                        free(arg);
                        pthread_exit((int *)1);
                    }
                } else {
                    printf("Invalid request\n");
                    if (write(new_socket, empty_data, sizeof(empty_data)) == -1) {
                        fprintf(stderr, "Failed to write to the socket\n");
                        close(new_socket);
                        free(arg);
                        pthread_exit((int *)1);
                    }
                    break;
                }
                //break if recevie flag 0
                if (recv_buf[RQS_PERSISTENT_FLAG] == 0) break;
            } else {
                printf("Invalid request\n");
                if (write(new_socket, empty_data, sizeof(empty_data)) == -1) {
                    fprintf(stderr, "Failed to write to the socket\n");
                    close(new_socket);
                    free(arg);
                    pthread_exit((int *)1);
                }
                break;
            }
        } else if (read_size < 0) {
            fprintf(stderr, "Failed to read from socket\n");
            close(new_socket);
            free(arg);
            pthread_exit((int *)1);
        }
    }
    printf("close connection from %s:%d\n", client_ip, client_port);
    if (close(new_socket)) {
        fprintf(stderr, "Failed to close the socket\n");
        free(arg);
        pthread_exit((int *)1);
    }
    free(arg);
    return NULL;
}

int main(int argc, char *argv[]) {
    // process input arguments
    if (argc != 2) {
        fprintf(stderr, "Usage: ./server <Server Port>\n");
        exit(1);
    }
    //init mutex lock
    if (pthread_mutex_init(&mutex, NULL)) {
        fprintf(stderr, "Failed to init mutex lock\n");
        exit(1);
    }

    // socket create and verification
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Failed to create the socket\n");
        exit(1);
    }

    // assign IP, PORT
    short server_port = atoi(argv[1]);
    int IP = INADDR_ANY;

    struct sockaddr_in servAddress;
    servAddress.sin_family = AF_INET;
    servAddress.sin_port = htons(server_port);
    servAddress.sin_addr.s_addr = htonl(IP);

    // Binding newly created socket to given IP
    if (bind(sock, (struct sockaddr *)&servAddress, sizeof(servAddress))) {
        perror("Failed to bind socket thread");
        exit(1);
    }

    // Now server is ready to listen
    if (listen(sock, MAX_CONNECTIONS)) {
        perror("Failed to listen on the socket\n");
        exit(1);
    }
    printf("server is listening\n");

    while (1) {
        // Accept the data packet from client
        struct sockaddr_in clientAddress;
        pthread_t new_thread;
        socklen_t size = sizeof(struct sockaddr_in);
        struct thread_args *thread_arg;
        thread_arg = malloc(sizeof(struct thread_args));
        if ((thread_arg->clientfd = accept(sock, (struct sockaddr *)&clientAddress, &size)) == -1) {
            fprintf(stderr, "Failed to accpet socket.\n");
            free(thread_arg);
            continue;
        }
        thread_arg->client_ip = inet_ntoa(clientAddress.sin_addr);
        thread_arg->client_port = clientAddress.sin_port;
        printf("open connection from %s:%d\n", thread_arg->client_ip, thread_arg->client_port);

        if (pthread_create(&new_thread, NULL, socket_thread, thread_arg) != 0) {
            fprintf(stderr, "Failed to create socket thread.\n");
            close(thread_arg->clientfd);
            free(thread_arg);
            continue;
        }

        pthread_detach(new_thread);
    }
    exit(EXIT_SUCCESS);
}
