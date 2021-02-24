#include "mapreduce.h"

int main(int argc, char *argv[])
{

	//TODO: number of argument check
	//Check if the arugments are inputed correct, it must be 4 arugments.
	if (argc != 4)
	{
		printf("Incorrect Arguments");
		return 1;
	}
	int nMappers = strtol(argv[1], NULL, 10);
	int nReducers = strtol(argv[2], NULL, 10);
	//Error checks
	if (nMappers < nReducers)
	{
		printf("#mappers need to be greater than #reducers");
		return 1;
	}
	else if (nMappers > MaxNumProcesses)
	{
		printf("#mappers need to be smaller than or equal to 20");
		return 1;
	}
	else if (nReducers > MaxNumProcesses)
	{
		printf("#reducers need to be smaller than or equal to 20");
		return 1;
	}

	inputFileDir = argv[3];
	if (!isValidDir(inputFileDir))
		exit(EXIT_FAILURE);

	bookeepingCode();

	// TODO: spawn mappers
	int i;
	char buffer[20]; //we need to use this buffer convert the id to string for mappers.
	char buffer2[20];//we need to use this buffer convert the id to string for reducers.
	pid_t mappers; 
	pid_t reducers;
	for (i = 0; i < nMappers; i++)
	{	//spawning mapper proccesses equal to the amount of nMappers.
		if ((mappers = fork()) == 0) 
		{	//converting id to string using sprintf and put it in buffer
			sprintf(buffer, "%d", i + 1);
			//using exec() to run mapper.c with inputs
			execl("./mapper", "./mapper", buffer, argv[1], inputFileDir, NULL);
			printf("Failed to create mappers");
			return 1;
		}
		else if (mappers < 0)
		{	//error check
			printf("Fork mappers failed");
			return 1;
		}
	}
	// TODO: wait for all children to complete execution
	while (wait(NULL) > 0)
		;
	// TODO: spawn reducers
	for (i = 0; i < nReducers; i++)
	{	//spawning mapper proccesses equal to the amount of nMappers.
		if ((reducers = fork()) == 0)
		{	//converting id to string using sprintf and put it in buffer2
			sprintf(buffer2, "%d", i + 1);
			//using exec() to run reducer.c with inputs
			execl("./reducer", "./reducer", buffer2, argv[2], inputFileDir, NULL);
			printf("Failed to create reducers");
			return 1;
		}
		else if (reducers < 0)
		{	//error check
			printf("Fork reducers failed");
			return 1;
		}
	}
	// TODO: wait for all children to complete execution
	while (wait(NULL) > 0)
		;
	return EXIT_SUCCESS;
}
