#include "producer.h"

/**
 *
 * Producer thread will read from the file and write data to 
 * the end of the shared queue
 */

void *producer(void *arg) {
    // open the file and read its content line by line
    // Send data to the shared queue
    // When reaching the end of the file, send EOF message to the sha
    char *path = (char *)arg;
    FILE *file = getFilePointer(path);
    // setting up linked list
    struct node_t *new_node;
    struct node_t *current_node;
    current_node = head_node;
    char readbuf[chunkSize];
    int word_size;
    int line = 0;
    char *word_hold;
	
    if ((strcmp(option, "-p") == 0) || (strcmp(option, "-bp") == 0)) {
        fprintf(fp, "producer\n");
    }
	//continue to read while the file isn't empty
    while ((word_size = getLineFromFile(file, readbuf, chunkSize)) > 0) {
    	//creating a new node and data
        new_node = (struct node_t *)malloc(sizeof(struct node_t));
        word_hold = malloc(sizeof(char) * word_size + 1);
        // locking the critial section
        if (pthread_mutex_lock(&mutex)) {
            fprintf(stderr, "Producer failed to obtain the mutex lock\n");
            exit(1);
        }
        strcpy(word_hold, readbuf);
        //if the bounded option is enabled, we will wait if the queue size is full
        if (boundedBuf) {
            while (list_size >= queueSize) {
                if (pthread_cond_wait(&empty_cond, &mutex)) {
                    fprintf(stderr, "Producer failed to wait for a slot in queue\n");
                    exit(1);
                }
            }
        }
        //adding a new node to the linked-list
        new_node->word = word_hold;
        new_node->next = NULL;
        if (head_node->next != NULL) {
            current_node->next = new_node;
            current_node = new_node;
        } else {
            head_node->next = new_node;
            current_node = head_node->next;
        }
        list_size++;
        // unlocking the critial section and signal the consumers
        if (pthread_cond_signal(&full_cond)) {
            fprintf(stderr, "Producer failed to signal the consumers\n");
            exit(1);
        }
        if (pthread_mutex_unlock(&mutex)) {
            fprintf(stderr, "Producer failed to release the lock\n");
            exit(1);
        }
        //print out the producer log if the option is enabled.
        if ((strcmp(option, "-p") == 0) || (strcmp(option, "-bp") == 0)) {
            fprintf(fp, "producer: %d\n", line);
            line++;
        }
    }
    //trigger the done variable
    done = 1;
    if (pthread_cond_broadcast(&full_cond)) {
        fprintf(stderr, "Producer failed to send the EOF notification\n");
        exit(1);
    }
    if ((strcmp(option, "-p") == 0) || (strcmp(option, "-bp") == 0)) {
        fprintf(fp, "producer: -1\n");
    }
    // cleanup and exit
    fclose(file);

    return NULL;
}
