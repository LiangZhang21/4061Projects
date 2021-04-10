#include "consumer.h"

/**
 * parse lines from the queue, and count words by word length
 */
void parse(int id) {
    if ((strcmp(option, "-p") == 0) || (strcmp(option, "-bp") == 0)) {
        fprintf(fp, "consumer %d: %d\n", id, line_num);
        line_num++;
    }
    char *line;
    struct node_t *temp;
    temp = head_node->next;
    if (temp->word[0] != '\n') {
        char *token = strtok_r(temp->word, "\n", &line);
        token = strtok_r(token, " ", &line);
        while (token != NULL) {
            finalDS[strlen(token) - 1]++;
            token = strtok_r(NULL, " ", &line);
        }
    }
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
        if (pthread_mutex_lock(&mutex)) {
			fprintf(stderr, "Consumer failed to obtain the lock\n");
			exit(1);
		}
        while ((list_size <= 0) && !done) {
            if (pthread_cond_wait(&full_cond, &mutex)) {
				fprintf(stderr, "Consumer failed to wait for an item\n");
				exit(1);
			}
		}
        if ((list_size <= 0) && done) {
            if (pthread_mutex_unlock(&mutex)) {
				fprintf(stderr, "Consumer failed to release the lock\n");
				exit(1);
			}
            break;
        }
        parse(id);
        list_size--;
        if (boundedBuf) {
            if (pthread_cond_signal(&empty_cond)) {
				fprintf(stderr, "Consumer failed to signal the producer\n");
				exit(1);
			}
        }
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
