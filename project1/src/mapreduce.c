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
	char buffer2[20];
	pid_t mappers;
	pid_t reducers;
	for(i=0; i<nMappers; i++){
		if((mappers = fork()) == 0){
			sprintf(buffer,"%d", i+1);
			execl("./mapper", "./mapper", buffer, argv[1], inputFileDir, NULL);
		}else if(mappers < 0){
			perror("Failed");
		}
	}
	// TODO: wait for all children to complete execution
	while(wait(NULL) > 0);	
	// TODO: spawn reducers
	for(i=0; i<nReducers; i++){
		if((reducers = fork()) == 0){
			sprintf(buffer2,"%d", i+1);
			execl("./reducer", "./reducer",buffer2, argv[2], inputFileDir,NULL);
		}else if(reducers < 0){
			perror("Failed");
		}
	}
	// TODO: wait for all children to complete execution
	while(wait(NULL) > 0);
	return EXIT_SUCCESS;
}
