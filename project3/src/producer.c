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
    int word_size;
    while((word_size = getLineFromFile(fp, readbuf, chunkSize)) > 0){	
    	if(strcmp(readbuf,"\n") != 0){
			temp = (struct node_t*)malloc(sizeof(struct node_t));
			char* word_hold;
			word_hold = malloc(sizeof(char)*word_size);
			strcpy(word_hold, readbuf); 
			//lock
			pthread_mutex_lock(&mutex);
			temp -> word = word_hold;
			last -> next = temp;
			last = temp;
			//printf("%s\n", last->word);
			//unlock
			pthread_mutex_unlock(&mutex); 	
    	}	
    	
    }
    
    // cleanup and exit
    return NULL; 
}





