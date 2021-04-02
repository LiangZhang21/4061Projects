#include "consumer.h"

/**
 * parse lines from the queue, and count words by word length
 */
void parse(int ID){
	char *line;
	char log_buf2[200];
	struct node_t *temp;
	temp = head_node -> next;
	//printf("consumed: %s\n", temp -> word);
	line = malloc(sizeof(char)*strlen(temp->word)+1);
	strcpy(line, temp -> word);
	char* token = strtok_r(line, "\n", &line);
	token = strtok_r(token, " ", &line);
	while(token != NULL){
		finalDS[strlen(token)-1]++;
		token = strtok_r(NULL, " ", &line);
	}
	head_node -> next = temp -> next;	
	int id = ID;
	if((strcmp(option, "-p") == 0) || (strcmp(option, "-bp") == 0)){			
		sprintf(log_buf2, "consumer %d: %d\n", id-1, line_num);
		write(fd, log_buf2, sizeof(char) * strlen(log_buf2));	
		line_num++;	
	} 	
	
}

// consumer function
void *consumer(void *arg){
    char log_buf[200];
    int id = (int*)arg;
    //printf("id: %d\n", id);
    //TODO: keep reading from queue and process the data
    // feel free to change   
    if((strcmp(option, "-p") == 0) || (strcmp(option, "-bp") == 0)){
    	if(line_num == 0){
 	   		sprintf(log_buf, "consumer %d\n", id-1);
			write(fd, log_buf, sizeof(char) * strlen(log_buf));
		}			
	}	

    while(1){
    	pthread_mutex_lock(&mutex);
    	while((list_size <= 0) && !done)
    		pthread_cond_wait(&full_cond, &mutex);
    	if((list_size <= 0) && done){
    		pthread_mutex_unlock(&mutex);
    		break;  	
    	}
        parse(id);
        list_size--;
        pthread_mutex_unlock(&mutex);
        if(boundedBuf){
        	pthread_cond_signal(&empty_cond); 
        }      
    }
    

    //TODO: update the global array
    
    return NULL; 
}


