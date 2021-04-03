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
    struct node_t *current_node;
    current_node = head_node;
    char readbuf[chunkSize];
    int word_size;
    char log_buf[200];
    int line = 0;
    char* word_hold;
    
    if((strcmp(option, "-p") == 0) || (strcmp(option, "-bp") == 0) ){
    	if(line == 0){
			strcpy(log_buf, "producer\n");
			write(fd, log_buf, sizeof(char) * strlen(log_buf));
		}			
	}	

    while((word_size = getLineFromFile(fp, readbuf, chunkSize)) > 0){	
    	if(strcmp(readbuf,"\n") != 0){
			new_node = (struct node_t*)malloc(sizeof(struct node_t));		
			word_hold = malloc(sizeof(char)*word_size+1);		
			//lock
			pthread_mutex_lock(&mutex);
			strcpy(word_hold, readbuf); 
			if(boundedBuf){
				while(list_size >= queueSize)	
					pthread_cond_wait(&empty_cond, &mutex);
			}
			new_node -> word = word_hold;
			if(head_node -> next != NULL){
				current_node -> next = new_node;
				current_node = new_node;				
			}else{
				head_node -> next = new_node;
				current_node = head_node -> next;
			}
			list_size++;	
			//printf("produced: %s\n", current_node->word);		
			//unlock
			pthread_cond_signal(&full_cond);
			pthread_mutex_unlock(&mutex); 			
    	}
    	if((strcmp(option, "-p") == 0) || (strcmp(option, "-bp") == 0) ){			
			sprintf(log_buf, "producer: %d\n", line);
			write(fd, log_buf, sizeof(char) * strlen(log_buf));	
			line++;	
		} 	
    }
    done = 1;
    pthread_cond_broadcast(&full_cond);
    // cleanup and exit
    fclose(fp);
    
    return NULL; 
}





