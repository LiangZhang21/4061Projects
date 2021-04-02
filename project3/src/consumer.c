#include "consumer.h"

/**
 * parse lines from the queue, and count words by word length
 */
void parse(){
	char *line;
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
}

// consumer function
void *consumer(void *arg){
    
    //TODO: keep reading from queue and process the data
    // feel free to change
    while(1){
    	pthread_mutex_lock(&mutex);
    	while((list_size <= 0) && !done)
    		pthread_cond_wait(&full_cond, &mutex);
    	if((list_size <= 0) && done){
    		pthread_mutex_unlock(&mutex);
    		break;  	
    	}
        parse();
        list_size--;
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&empty_cond);       
    }

    //TODO: update the global array
    
    return NULL; 
}


