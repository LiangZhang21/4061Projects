#include "producer.h"

/**
 *
 * Producer thread will read from the file and write data to 
 * the end of the shared queue
 */
void *producer(void *arg){
    //TODO: open the file and read its content line by line
    //Send data to the shared queue
    //When reaching the end of the file, send EOF message to the sha 
    char* path = (char*)arg;
    FILE* fp = getFilePointer(path);
    //setting up linked list    
    struct node_t *temp;
    struct node_t *last;
    last = head_node;
    char readbuf[chunkSize];
    while(getLineFromFile(fp, readbuf, chunkSize) > 0){
    	pthread_mutex_lock(&mutex);
    	temp = (struct node_t*)malloc(sizeof(struct node_t));
    	char word_hold[400];
    	strcpy(word_hold, readbuf); 
    	temp -> word = word_hold;
    	temp -> next = NULL;
    	last -> next = temp;
    	last = temp;
    	printf("%s\n", temp -> word);
    	//unlock
    	pthread_mutex_unlock(&mutex); 		
    	
    }
    
    // cleanup and exit
    return NULL; 
}





