#include "mapper.h"

/**
 * Write word count that is stored in an intermediate data structure to files.
 * The file should be in a corresponding folder in output/IntermediateData/ 
 */
void writeInterDSToFiles(void)
{
	int i;
	char buf[30];
	char buf2[maxFileNameLength];
	for (i = 0; i < MaxWordLength; i++)
	{
		//using buf to display the word length vs counts
		sprintf(buf, "%d %d", i + 1, interDS[i]);
		//using buf2 for string append for the path and file.
		sprintf(buf2, "/%d/m%d.txt", i + 1, mapperID);
		char temp[maxFileNameLength];
		strcpy(temp, intermediateDir);
		strcat(temp, buf2);
		writeLineToFile(temp, buf);
	}
}

/**
 * parse lines from pipes, and count words by word length
 */
void parse(char *line)
{
	char buffer[chunkSize];
	buffer[0] = '\0';
	strcat(buffer, line);
	char *token = strtok(buffer, "\n"); //Using token to split the line
	token = strtok(token, " ");
	while (token != NULL)
	{
		//add the value of the words count into an array where its indices is its length.
		interDS[strlen(token) - 1]++;
		token = strtok(NULL, " ");
	}
}

int main(int argc, char *argv[])
{

	mapperID = strtol(argv[1], NULL, 10);

	// you can read lines from pipes (from STDIN) (read lines in a while loop)
	//feel free to change
	char buf[chunkSize];
	//recevie from the pipe and call the parse() function
	int bytesRead;
	while ((bytesRead = read(STDIN_FILENO, buf, chunkSize)) > 0)
	{
		parse(buf);
	}
	if (bytesRead == -1)
	{
		fprintf(stderr, "ERROR: read failed\n");
		return EXIT_FAILURE;
	}

	if (close(STDIN_FILENO))
	{
		fprintf(stderr, "ERROR: close failed\n");
		exit(EXIT_FAILURE);
	};

	writeInterDSToFiles();

	return EXIT_SUCCESS;
}
