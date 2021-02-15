#include "mapper.h"

/**
 * Write word count that is stored in a intermeidate data structure to files.
 * The file should be in a corresponding folder in output/IntermediateData/ 
 */
void writeInterDSToFiles(void) {
}

/**
 * Read lines from files, and count words by word length
 */
void map(char * inputFileName) {
}

int main(int argc, char *argv[]) {

    mapperID = strtol(argv[1], NULL, 10);
    int nMappers = strtol(argv[2], NULL, 10);
    inputFileDir = argv[3];

    //getMapperTasks function returns a list of input file names that this mapper should process
    char *myTasks[MaxNumInputFiles] = {NULL};
    int nTasks = getMapperTasks(nMappers, mapperID, inputFileDir, &myTasks[0]);

    int tIdx;
    for (tIdx = 0; tIdx < nTasks; tIdx++) {
        printf("mapper[%d] - %s\n", mapperID, myTasks[tIdx]);
        map(myTasks[tIdx]);
        free(myTasks[tIdx]);
    }

    writeInterDSToFiles();

    return EXIT_SUCCESS;
}