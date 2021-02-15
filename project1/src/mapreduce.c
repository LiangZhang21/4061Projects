#include "mapreduce.h"

int main(int argc, char *argv[]) {
	
	//TODO: number of argument check

	int nMappers 	= strtol(argv[1], NULL, 10);
	int nReducers 	= strtol(argv[2], NULL, 10);

    inputFileDir = argv[3];
    if(!isValidDir(inputFileDir))
        exit(EXIT_FAILURE);

	bookeepingCode();

	// TODO: spawn mappers	

	// TODO: wait for all children to complete execution

	// TODO: spawn reducers

	// TODO: wait for all children to complete execution

	return EXIT_SUCCESS;
}