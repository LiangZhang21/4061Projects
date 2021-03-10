#include "reducer.h"

/**
 * Write final word count to files.
 * The file should be in a corresponding folder in output/FinalData/ 
 */
void writeFinalDSToFiles(void) {
	int i;
	char buffer[40];
	char buffer2[maxFileNameLength];
	for (i = 0; i < MaxWordLength; i++)
	{	//iterating through the array and using buffer to display the lengths and counts.
		sprintf(buffer, "%d %d", i + 1, finalDS[i]);
		//using buffer and temp for string append for the directory path and create the text file.
		sprintf(buffer2, "/%d.txt", i + 1);
		char temp[maxFileNameLength];
		strcpy(temp, finalDir);
		strcat(temp, buffer2);
		if (finalDS[i] != 0)
		{	//outputing the final to the text file.
			writeLineToFile(temp, buffer);
		}
	}
}


/**
 * Read lines from files, and calculate a total count for a specific word length
 */
void reduce(char * intermediateFileName) {
	//opening up the file
	FILE *fp = getFilePointer(intermediateFileName);
	char buffer[chunkSize];
	char index[10];
	//get the line from the file and put it into a buffer, run until the end of file.
	while (getLineFromFile(fp, buffer, chunkSize) > 0)
	{	//using strtok to break the " " in the file.
		char *token = strtok(buffer, " ");
		strcpy(index, token);
		token = strtok(NULL, " ");
		//summing the counts and put it into the finalDS array.
		finalDS[atoi(index) - 1] += atoi(token);
	}
}

int main(int argc, char *argv[]) {

	// initialize 
	reducerID = strtol(argv[1], NULL, 10);
	int nReducers = strtol(argv[2], NULL, 10);

    //getReducerTasks function returns a list of intermediate file names that this reducer should process
    char *myTasks[MaxNumIntermediateFiles] = {NULL};

    //TODO: you can write your own getReducerTasks in utils file or change this however you like.
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
