#include "reducer.h"

/**
 * Write final word count to files.
 * The file should be in a corresponding folder in output/FinalData/ 
 */
void writeFinalDSToFiles(void) {
}


/**
 * Read lines from files, and calculate a total count for a specific word length
 */
void reduce(char * intermediateFileName) {
}

int main(int argc, char *argv[]) {

	// initialize 
	reducerID = strtol(argv[1], NULL, 10);
	int nReducers = strtol(argv[2], NULL, 10);

    //getReducerTasks function returns a list of intermediate file names that this reducer should process
    char *myTasks[MaxNumIntermediateFiles] = {NULL};
    int nTasks = getReducerTasks(nReducers, reducerID, intermediateDir, &myTasks[0]);

    int tIdx;
    for (tIdx = 0; tIdx < nTasks; tIdx++) {
        printf("reducer[%d] - %s\n", reducerID, myTasks[tIdx]);
        reduce(myTasks[tIdx]);
        free(myTasks[tIdx]);
    }

    writeFinalDSToFiles();

	return EXIT_SUCCESS;
}