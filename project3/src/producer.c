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
    struct node_t *new_node;
    struct node_t *last;
    last = head_node;
    char readbuf[chunkSize];
    int word_size;
    while((word_size = getLineFromFile(fp, readbuf, chunkSize)) > 0){	
    	if(strcmp(readbuf,"\n") != 0){
			new_node = (struct node_t*)malloc(sizeof(struct node_t));
			char* word_hold;
			word_hold = malloc(sizeof(char)*word_size+1);
			strcpy(word_hold, readbuf); 
			//lock
			pthread_mutex_lock(&mutex);
			while(list_size >= item_hold_size)	
				pthread_cond_wait(&empty_cond, &mutex);
			new_node -> word = word_hold;
			last -> next = new_node;
			if(head_node -> next != NULL){
				last = new_node;			
			}else{
				head_node -> next = new_node;
				last = head_node -> next;
			}
			list_size++;
			//printf("produced: %s\n", last->word);
			//unlock
			pthread_mutex_unlock(&mutex); 	
			pthread_cond_signal(&full_cond);	
    	}	  	
    }
    done = 1;
    pthread_cond_broadcast(&full_cond);
    // cleanup and exit
    return NULL; 
}





