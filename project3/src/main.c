#include "header.h"

/**
 * Write final word count to a single file.
 * The path name should be output/result.txt
 */

void writeFinalDSToFiles(void) {
}

void printList(){
	struct node_t *temp;
	temp = head_node;
	while( (temp = temp -> next) != NULL){
		printf("word: %s\n", temp -> word);
	}
}

int main(int argc, char *argv[]){
    
    //TODO: Argument check
	if((argc <= 3) && (argc >= 5)){
		printf("Incorrect Arguments");
		return 1;
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

    //TODO: create producer and consumer threads
    //create producer
	if(pthread_create(&th[0], NULL, producer, path) != 0){
		perror("Failed to create producer thread");
	}
		
	for(int i = 1; i <= consumers; i++){
		//create consumers
		if(pthread_create(&th[i], NULL, consumer, NULL) != 0){
				perror("Failed to create consumer thread");
		}	

	}
	
    //TODO: wait for all threads to complete execution
    for(int i = 0; i <= consumers; i++){
    	if(pthread_join(th[i], NULL) != 0){
    		perror("Failed to join thread");
    	}
    }
    
    //Write the final output
    writeFinalDSToFiles();
    
    printList();
    
    return 0; 
}

