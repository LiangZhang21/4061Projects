#include "mapreduce.h"
#include "unistd.h"

int getNames(char *path);

int main(int argc, char *argv[])
{

	//TODO: number of argument check
	if (argc != 4)
	{
		printf("Incorrect Arguments");
		return 1;
	}

	int nMappers = strtol(argv[1], NULL, 10);
	int nReducers = strtol(argv[2], NULL, 10);
	//Error checking
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

	//TODO: Traverse Input File Directory (Hint: Recursively find all the text files inside directory)
	// and create MapperInput directory which will contain MapperID.txt files

	//creating the input directory
	int i;
	int checker;
	char *dirname = "MapperInput";

	// remove the existing MapperInput directory before creating it again
	pid_t pid;
	if ((pid = fork()) == 0)
	{
		execlp("rm", "rm", "-rf", dirname, NULL);
		fprintf(stderr, "ERROR: exec failed\n");
		exit(EXIT_FAILURE);
	}
	else if (pid > 0)
	{
		wait(NULL);
	}
	else
	{
		fprintf(stderr, "ERROR: fork failed\n");
		exit(EXIT_FAILURE);
	}

	checker = mkdir(dirname, 0777);
	file_names = malloc(MaxNumInputFiles * sizeof(char *));
	getNames(inputFileDir);

	//open MapperInput folder and create the stream mapper text files.
	char sMap_buf[20];
	char sMap_write_buf[maxFileNameLength];
	for (i = 0; i < nMappers; i++)
	{
		char stream_mapper[maxFileNameLength] = "MapperInput/Mapper";
		sprintf(sMap_buf, "%d.txt", i + 1);
		strcat(stream_mapper, sMap_buf);
		int fd = open(stream_mapper, O_WRONLY | O_CREAT | O_TRUNC, 0777);
		if (fd == -1)
		{ //Error checking
			fprintf(stderr, "Failed to create and open the file.\n");
			exit(EXIT_FAILURE);
		}
		//Distribute the files among each stream mappers
		int j;
		for (j = i; j < textfile_counter; j += nMappers)
		{
			strcpy(sMap_write_buf, file_names[j]);
			strcat(sMap_write_buf, "\n");
			int bytesWrite = write(fd, sMap_write_buf, sizeof(char) * strlen(file_names[j]) + 1);
			if (bytesWrite == -1)
			{
				fprintf(stderr, "ERROR: write failed\n");
				exit(EXIT_FAILURE);
			}
			free(file_names[j]);
		}
		if (close(fd) == -1)
		{
			fprintf(stderr, "ERROR: close failed\n");
			exit(EXIT_FAILURE);
		}
	}
	free(file_names);

	//TODO: spawn stream and mapper processes
	pid_t s_mappers;
	pid_t mappers;
	char stream_buf[20];
	char buffer[20]; //we need to use this buffer convert the id to string for mappers.
	int fd[2];		 //creating a pipe
	for (i = 0; i < nMappers; i++)
	{
		if (pipe(fd) == -1)
		{
			fprintf(stderr, "ERROR: pipe failed\n");
			exit(EXIT_FAILURE);
		}
		//spawn stream mappers
		if ((s_mappers = fork()) == 0)
		{
			if (dup2(fd[1], STDOUT_FILENO) == -1)
			{
				fprintf(stderr, "ERROR: dup2 failed\n");
				exit(EXIT_FAILURE);
			};
			//close fds that won't be used
			if (close(fd[0]))
			{
				fprintf(stderr, "ERROR: close failed\n");
				exit(EXIT_FAILURE);
			};
			if (close(fd[1]))
			{
				fprintf(stderr, "ERROR: close failed\n");
				exit(EXIT_FAILURE);
			};
			sprintf(stream_buf, "%d", i + 1);
			execl("./stream", "./stream", stream_buf, NULL);
			fprintf(stderr, "Failed to create mappers\n");
			exit(EXIT_FAILURE);
		}
		else if (s_mappers < 0)
		{
			fprintf(stderr, "Fork s_mappers failed\n");
			exit(EXIT_FAILURE);
		}
		//spawn mappers
		if ((mappers = fork()) == 0)
		{
			if (dup2(fd[0], STDIN_FILENO) == -1)
			{
				fprintf(stderr, "ERROR: dup2 failed\n");
				exit(EXIT_FAILURE);
			};
			//close fds that won't be used
			if (close(fd[0]))
			{
				fprintf(stderr, "ERROR: close failed\n");
				exit(EXIT_FAILURE);
			};
			if (close(fd[1]))
			{
				fprintf(stderr, "ERROR: close failed\n");
				exit(EXIT_FAILURE);
			};
			//converting id to string using sprintf and put it in buffer
			sprintf(buffer, "%d", i + 1);
			//using exec() to run mapper.c with inputs
			execl("./mapper", "./mapper", buffer, NULL);
			fprintf(stderr, "Failed to create mappers\n");
			exit(EXIT_FAILURE);
		}
		else if (mappers < 0)
		{ //error checking
			fprintf(stderr, "Fork mappers failed\n");
			exit(EXIT_FAILURE);
		}
		//closing the pipe
		if (close(fd[0]))
		{
			fprintf(stderr, "ERROR: close failed\n");
			exit(EXIT_FAILURE);
		};
		if (close(fd[1]))
		{
			fprintf(stderr, "ERROR: close failed\n");
			exit(EXIT_FAILURE);
		};
	}

	// TODO: wait for all children to complete execution
	while (wait(NULL) > 0)
		;

	// TODO: spawn reducers
	char buffer2[20]; //we need to use this buffer convert the id to string for reducers.
	pid_t reducers;
	for (i = 0; i < nReducers; i++)
	{ //spawning mapper proccesses equal to the amount of nMappers.
		if ((reducers = fork()) == 0)
		{ //converting id to string using sprintf and put it in buffer2
			sprintf(buffer2, "%d", i + 1);
			//using exec() to run reducer.c with inputs
			execl("./reducer", "./reducer", buffer2, argv[2], NULL);
			fprintf(stderr, "Failed to create reducers\n");
			exit(EXIT_FAILURE);
		}
		else if (reducers < 0)
		{ //error checking
			fprintf(stderr, "Fork reducers failed\n");
			exit(EXIT_FAILURE);
		}
	}
	// TODO: wait for all children to complete execution
	while (wait(NULL) > 0)
		;
	return EXIT_SUCCESS;
}

//This function is used to fill in the file_names array and textfile_counter
int getNames(char *path)
{
	int file_counter;
	DIR *dir = opendir(path); //open the path directory
	struct stat stat_buf;
	struct dirent *entry;
	if (dir == NULL)
	{ //error checking
		fprintf(stderr, "Failed to open directory\n");
		exit(EXIT_FAILURE);
	}
	//go in to the directory
	while ((entry = readdir(dir)) != NULL)
	{ //Skipping "." and ".."
		if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
			continue;
		//appending file path
		char next[strlen(path) + strlen(entry->d_name) + 2];
		next[0] = '\0';
		strcat(next, path);
		strcat(next, "/");
		strcat(next, entry->d_name);
		
		if ((entry->d_type == DT_DIR))
		{
			getNames(next);
		}
		else if((entry->d_type == DT_REG)) //Any type other than regular will not go in, including symbolic linked.
		{	
			stat(next, &stat_buf);
			//check if the file is hard linked or not. If so, skip it.
			if(stat_buf.st_nlink == 1){
				//adding the path into the files_names array and increment the counter
				char *buf = malloc(sizeof(char) * maxFileNameLength); // buf is freed in the main function
				buf[0] = '\0';
				strcat(buf, path);
				strcat(buf, "/");
				strcat(buf, entry->d_name);
				file_names[textfile_counter] = buf;
				textfile_counter++;
			}
		}
	}
	if (closedir(dir))
	{
		fprintf(stderr, "ERROR: closing directory failed\n");
		exit(EXIT_FAILURE);
	}
}
