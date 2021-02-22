#include "mapper.h"

/**
 * Write word count that is stored in a intermeidate data structure to files.
 * The file should be in a corresponding folder in output/IntermediateData/ 
 */
void writeInterDSToFiles(void)
{
	int i;
	char buffer[30];
	char buffer2[maxFileNameLength];
	for (i = 0; i < MaxWordLength; i++)
	{
		sprintf(buffer, "%d %d", i + 1, interDS[i]);
		sprintf(buffer2, "/%d/m_%d.txt", i + 1, mapperID);
		char temp[maxFileNameLength];
		strcpy(temp, intermediateDir);
		strcat(temp, buffer2);
		writeLineToFile(temp, buffer);
	}
}

/**
 * Read lines from files, and count words by word length
 */
void map(char *inputFileName)
{
	FILE *fp = getFilePointer(inputFileName);
	char buffer[chunkSize];
	while (getLineFromFile(fp, buffer, chunkSize) > 0)
	{
		char *token = strtok(buffer, "\n");
		token = strtok(buffer, " ");
		while (token != NULL)
		{
			interDS[strlen(token) - 1]++;
			token = strtok(NULL, " ");
		}
	}
}

int main(int argc, char *argv[])
{

	mapperID = strtol(argv[1], NULL, 10);
	int nMappers = strtol(argv[2], NULL, 10);
	inputFileDir = argv[3];

	//getMapperTasks function returns a list of input file names that this mapper should process
	char *myTasks[MaxNumInputFiles] = {NULL};
	int nTasks = getMapperTasks(nMappers, mapperID, inputFileDir, &myTasks[0]);

	int tIdx;
	for (tIdx = 0; tIdx < nTasks; tIdx++)
	{
		printf("mapper[%d] - %s\n", mapperID, myTasks[tIdx]);
		map(myTasks[tIdx]);
		free(myTasks[tIdx]);
	}

	writeInterDSToFiles();

	return EXIT_SUCCESS;
}
