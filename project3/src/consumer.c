#include "consumer.h"

/**
 * parse lines from the queue, and count words by word length
 */
void parse(int id) {
	//print out the log if the option are true
    if ((strcmp(option, "-p") == 0) || (strcmp(option, "-bp") == 0)) {
        fprintf(fp, "consumer %d: %d\n", id, line_num);
        //increment the line count
        line_num++;
    }
    char *line;
    struct node_t *temp;
    temp = head_node->next;
    //splitting the line and put the data into the array
    if (temp->word[0] != '\n') {
        char *token = strtok_r(temp->word, "\n", &line);
        token = strtok_r(token, " ", &line);
        while (token != NULL) {
            finalDS[strlen(token) - 1]++;
            token = strtok_r(NULL, " ", &line);
        }
    }
    //delete the used node
    head_node->next = temp->next;
    free(temp->word);
    free(temp);
}

// consumer function
void *consumer(void *arg) {
    int id = *(int *)arg - 1;
    // keep reading from queue and process the data
    if ((strcmp(option, "-p") == 0) || (strcmp(option, "-bp") == 0)) {
        fprintf(fp, "consumer %d\n", id);
    }

    while (1) {
    	//locking up the critial section
        if (pthread_mutex_lock(&mutex)) {
			fprintf(stderr, "Consumer failed to obtain the lock\n");
			exit(1);
		}
		//while the list still exits and produce isn't done it will continue to run
        while ((list_size <= 0) && !done) {
            if (pthread_cond_wait(&full_cond, &mutex)) {
				fprintf(stderr, "Consumer failed to wait for an item\n");
				exit(1);
			}
		}
		//if the producer is done then it will unlock.
        if ((list_size <= 0) && done) {
            if (pthread_mutex_unlock(&mutex)) {
				fprintf(stderr, "Consumer failed to release the lock\n");
				exit(1);
			}
            break;
        }
        //caling the parse function and pass in the id of consumers
        parse(id);
        list_size--;
        //if bounded buffer is enabled, notify the producer the queue size is no longer full
        if (boundedBuf) {
            if (pthread_cond_signal(&empty_cond)) {
				fprintf(stderr, "Consumer failed to signal the producer\n");
				exit(1);
			}
        }
        //unlocking the critial section
        if (pthread_mutex_unlock(&mutex)) {
			fprintf(stderr, "Consumer failed to release the lock\n");
			exit(1);
		}
    }

	if ((strcmp(option, "-p") == 0) || (strcmp(option, "-bp") == 0)) {
        fprintf(fp, "consumer %d: -1\n", id);
    }

    free(arg);
    return NULL;
}
