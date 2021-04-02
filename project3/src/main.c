#include "header.h"

/**
 * Write final word count to a single file.
 * The path name should be output/result.txt
 */

void writeFinalDSToFiles(void) {
	int i;
	char value_buf[200];
	for(i = 0; i < MaxWordLength; i++){
		sprintf(value_buf, "%d %d\n", i+1, finalDS[i]);
		//if(finalDS[i] != 0){ //enable this line if you want to ignore 0's
			writeLineToFile(finalDir, value_buf);
		//}
	}
}

int main(int argc, char *argv[]){
    
    //TODO: Argument check
	if((argc <= 3) && (argc >= 5)){
		printf("Incorrect Arguments");
		return 1;
	}else{
		if(argc == 4){
			option = argv[3];
			if((strcmp(argv[3], "-bp") == 0) || (strcmp(argv[3], "-b") == 0)){
				printf("Inccorect Arugment: Require queue size\n");
				return 1;
			}
		}else if(argc == 5){
			option = argv[3];
			queueSize = atoi(argv[4]);	
			if(queueSize <= 0 ){
				printf("Incorrect Arguments\n");
				return 1;
			}
			boundedBuf = 1;		
		}else{
			option = "";
			boundedBuf = 0;
		}
	}
	
    bookeepingCode();
    
    //TODO: Initialize global variables, like shared queue
	head_node = (struct node_t*) malloc(sizeof(struct node_t));
    head_node -> word = NULL;
    head_node -> next = NULL;
    
    int consumers = atoi(argv[1]);
    char* path = argv[2];
    pthread_t th[consumers+1];
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&full_cond, NULL);
    pthread_cond_init(&empty_cond, NULL);
    
    //item_hold_size = 20;
	list_size = 0;
	line_num = 0;
	done = false;
	//open log file
	if((strcmp(option, "-p") == 0) || (strcmp(option, "-bp") == 0)){
		fd = open(logDir, O_WRONLY | O_CREAT | O_TRUNC, 0777);
		if(fd == -1){
			fprintf(stderr, "Failed to open the file.\n");
			exit(EXIT_FAILURE);
		}
	}

    //TODO: create producer and consumer threads
    //create producer
	if(pthread_create(&th[0], NULL, producer, path) != 0){
		perror("Failed to create producer thread");
	}
		
	for(int i = 1; i <= consumers; i++){
		//create consumers
		int* id = malloc(sizeof(int));
		*id = i;
		if(pthread_create(&th[i], NULL, consumer, id) != 0){
				perror("Failed to create consumer thread");
		}	

	}

    //TODO: wait for all threads to complete execution
    for(int i = 0; i <= consumers; i++){
    	if(pthread_join(th[i], NULL) != 0){
    		perror("Failed to join thread");
    	}
    }
	
	char buf[200];
	if((strcmp(option, "-p") == 0) || (strcmp(option, "-bp") == 0)){
 	   	sprintf(buf, "producer: %d\n", -1);
		write(fd, buf, sizeof(char) * strlen(buf));		
	}    
    
    //Write the final output
    writeFinalDSToFiles();
    
    //printList();
    
    return 0; 
}

