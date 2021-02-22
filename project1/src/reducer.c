#include "reducer.h"

/**
 * Write final word count to files.
 * The file should be in a corresponding folder in output/FinalData/ 
 */
void writeFinalDSToFiles(void)
{
	int i;
	char buffer[40];
	char buffer2[maxFileNameLength];
	for (i = 0; i < MaxWordLength; i++)
	{
		sprintf(buffer, "%d %d", i + 1, finalDS[i]);
		sprintf(buffer2, "/%d.txt", i + 1);
		char temp[maxFileNameLength];
		strcpy(temp, finalDir);
		strcat(temp, buffer2);
		if (finalDS[i] != 0)
		{
			writeLineToFile(temp, buffer);
		}
	}
}
/**
 * Read lines from files, and calculate a total count for a specific word length
 */
void reduce(char *intermediateFileName)
{
	FILE *fp = getFilePointer(intermediateFileName);
	char buffer[chunkSize];
	char index[10];
	while (getLineFromFile(fp, buffer, chunkSize) > 0)
	{
		char *token = strtok(buffer, " ");
		strcpy(index, token);
		token = strtok(NULL, " ");
		finalDS[atoi(index) - 1] += atoi(token);
	}
}

int main(int argc, char *argv[])
{

	// initialize
	reducerID = strtol(argv[1], NULL, 10);
	int nReducers = strtol(argv[2], NULL, 10);

	//getReducerTasks function returns a list of intermediate file names that this reducer should process
	char *myTasks[MaxNumIntermediateFiles] = {NULL};
	int nTasks = getReducerTasks(nReducers, reducerID, intermediateDir, &myTasks[0]);

	int tIdx;
	for (tIdx = 0; tIdx < nTasks; tIdx++)
	{
		printf("reducer[%d] - %s\n", reducerID, myTasks[tIdx]);
		reduce(myTasks[tIdx]);
		free(myTasks[tIdx]);
	}

	writeFinalDSToFiles();

	return EXIT_SUCCESS;
}
