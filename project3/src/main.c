#include "header.h"

/**
 * Write final word count to a single file.
 * The path name should be output/result.txt
 */

void writeFinalDSToFiles(void) {
    int i;
    char value_buf[200];
    for (i = 0; i < MaxWordLength; i++) {
        sprintf(value_buf, "%d %d\n", i + 1, finalDS[i]);
        writeLineToFile(finalDir, value_buf);
    }
}

int main(int argc, char* argv[]) {
    // argument check
    if ((argc < 3) || (argc > 5)) {
        fprintf(stderr, "Incorrect arguments\n");
        return 1;
    } else {
        if (argc == 4) {
            option = argv[3];
            if ((strcmp(argv[3], "-bp") == 0) || (strcmp(argv[3], "-b") == 0)) {
                fprintf(stderr, "Incorrect arugment: require queue size\n");
                return 1;
            }
        } else if (argc == 5) {
            option = argv[3];
            queueSize = atoi(argv[4]);
            if (queueSize <= 0) {
                fprintf(stderr, "Incorrect arguments\n");
                return 1;
            }
            boundedBuf = 1;
        } else {
            option = "";
            boundedBuf = 0;
        }
    }

    bookeepingCode();

    // initialize global variables, like shared queue
    int consumers = atoi(argv[1]);
    if (consumers > 20) {
        fprintf(stderr, "Consumer number greater than the max of 20\n");
        return 1;
    }
    char* path = argv[2];
    head_node = (struct node_t*)malloc(sizeof(struct node_t));
    head_node->word = NULL;
    head_node->next = NULL;
    pthread_t th[consumers + 1];
    if (pthread_mutex_init(&mutex, NULL)) {
        fprintf(stderr, "Failed to init mutex lock\n");
        return 1;
    }
    if (pthread_cond_init(&full_cond, NULL)) {
        fprintf(stderr, "Failed to init conditional variable\n");
        return 1;
    }
    if (pthread_cond_init(&empty_cond, NULL)) {
        fprintf(stderr, "Failed to init conditional variable\n");
        return 1;
    }
    list_size = 0;
    line_num = 0;
    done = false;
    // open log file
    if ((strcmp(option, "-p") == 0) || (strcmp(option, "-bp") == 0)) {
        fp = fopen(logDir, "w");
    }

    // create producer and consumer threads
    // create producer
    if (pthread_create(&th[0], NULL, producer, path)) {
        fprintf(stderr, "Failed to create producer thread\n");
        return 1;
    }

    for (int i = 1; i <= consumers; i++) {
        // create consumers
        int* id = malloc(sizeof(int));
        *id = i;
        if (pthread_create(&th[i], NULL, consumer, id)) {
            fprintf(stderr, "Failed to create consumer thread\n");
            return 1;
        }
    }

    // wait for all threads to complete execution
    for (int i = 0; i <= consumers; i++) {
        if (pthread_join(th[i], NULL)) {
            fprintf(stderr, "Failed to join thread\n");
            return 1;
        }
    }

    // write the final output
    writeFinalDSToFiles();
    free(head_node);
    if ((strcmp(option, "-p") == 0) || (strcmp(option, "-bp") == 0)) {
        fclose(fp);
    }

    return 0;
}
