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
	{	//using buffer to display the word lengths vs counts
		sprintf(buffer, "%d %d", i + 1, interDS[i]);
		//using buffer2 for string append for the directory path and create the text file.
		sprintf(buffer2, "/%d/m%d.txt", i + 1, mapperID);
		char temp[maxFileNameLength];
		strcpy(temp, intermediateDir);
		strcat(temp, buffer2);
		//outputing to the text file.
		writeLineToFile(temp, buffer);
	}
}

/**
 * Read lines from files, and count words by word length
 */
void map(char *inputFileName)
{	//opening up the file
	FILE *fp = getFilePointer(inputFileName);
	char buffer[chunkSize];
	//get the line from the file and put it into a buffer, run until the end of file.
	while (getLineFromFile(fp, buffer, chunkSize) > 0)
	{	//using strtok to break each \n and " " in the file.
		char *token = strtok(buffer, "\n");
		token = strtok(token, " ");
		//keep running as long as the line is not empty
		while (token != NULL)
		{	//using an array to keep track the words, where the length is its indices and value is the count.	
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
