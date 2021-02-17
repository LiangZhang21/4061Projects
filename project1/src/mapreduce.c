#include "mapreduce.h"

int main(int argc, char *argv[]) {
	
	//TODO: number of argument check
	if(argc != 4 ){
		perror("Argument too short");
	}
	int nMappers 	= strtol(argv[1], NULL, 10);
	int nReducers 	= strtol(argv[2], NULL, 10);

    inputFileDir = argv[3];
    if(!isValidDir(inputFileDir))
        exit(EXIT_FAILURE);

	bookeepingCode();
	
	// TODO: spawn mappers	
	int i;
	char buffer[20];
	pid_t mappers;
	pid_t reducers;
	for(i=0; i<nMappers; i++){
		if((mappers = fork()) == 0){
			execl("./mapper.c", "./mapper.c", sprintf(buffer,"%d",i), argv[1], inputFileDir, NULL);
		}else if(mappers < 0){
			perror("Failed");
		}
	}
	// TODO: wait for all children to complete execution
	while(wait(NULL));	
	// TODO: spawn reducers
	for(i=0; i<nReducers; i++){
		if((reducers = fork()) == 0){
			execl("./reducer.c", "./reducer.c",sprintf(buffer,"%d",i), argv[2], inputFileDir,NULL);
		}else if(reducers < 0){
			perror("Failed");
		}
	}
	// TODO: wait for all children to complete execution
	while(wait(NULL));
	return EXIT_SUCCESS;
}
