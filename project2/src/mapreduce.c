#include "mapreduce.h"
#include "unistd.h"

int textfile_counter = 0;
char** file_names;

//This function is used to fill in the file_names array and textfile_counter
int getNames(char* path)
{
		int file_counter;
		DIR *dir = opendir(path); //open the path directory 
		struct dirent *entry;
		if (dir == NULL)
		{ //error check
			printf("Failed to open directory\n");
			return 1;
		}
		//go in to the directory
		while((entry = readdir(dir)) != NULL)
		{	//Skipping "." and ".."
			if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) continue;
			if((entry->d_type == DT_DIR))
			{
				char next[strlen(path) + strlen(entry->d_name) + 2];
				next[0] = '\0';
				strcat(next, path);
				strcat(next, "/");
				strcat(next, entry->d_name);
				getNames(next);
			}
			else 
			{
				//adding the path into the files_names array and increment the counter
				char *buf = malloc(sizeof(char) * 200);
				buf[0] = '\0';
				strcat(buf, path);
				strcat(buf, "/");
				strcat(buf, entry->d_name);
				file_names[textfile_counter] = buf;
				textfile_counter++;	
			}
		}
		closedir(dir);
}	

int main(int argc, char *argv[]) {
	
	//TODO: number of argument check
	if (argc != 4)
	{
		printf("Incorrect Arguments");
		return 1;
	}
	
	int nMappers 	= strtol(argv[1], NULL, 10);
	int nReducers 	= strtol(argv[2], NULL, 10);

    	inputFileDir = argv[3];
    	if(!isValidDir(inputFileDir))
        exit(EXIT_FAILURE);

	bookeepingCode();

	
	//TODO: Traverse Input File Directory (Hint: Recursively find all the text files inside directory)
	// and create MapperInput directory which will contain MapperID.txt files
		
	//creating the input directory
	int i;
	int checker;
	char* dirname = "MapperInput";
	
	checker = mkdir(dirname, 0777);
	file_names = malloc(100 * sizeof(char*));
	getNames(inputFileDir);
	
	//open MapperInput folder and create the stream mapper text files.
	char sMap_buf[20];
	char sMap_write_buf[200];
	for(i = 0; i < nMappers; i++)
	{
		char stream_mapper[200] = "MapperInput/Mapper";
		sprintf(sMap_buf, "%d.txt", i + 1);
		strcat(stream_mapper, sMap_buf);
		int fd = open(stream_mapper, O_WRONLY | O_CREAT, 0777);
		if(fd == -1)
		{ //Error check
			printf("Failed to create and open the file. \n");
			exit(1);
		}
		//Distribute the files among the into each stream mappers
		int j;
		for(j = i; j < textfile_counter; j+= nMappers)
		{
			strcpy(sMap_write_buf, file_names[j]);
			strcat(sMap_write_buf, "\n");
			write(fd, sMap_write_buf, sizeof(char)*strlen(file_names[j])+1);
			free(file_names[j]);
		}
	}
	free(file_names);
	



	//TODO: spawn stream and mapper processes
	pid_t s_mappers;
	pid_t mappers; 
	char stream_buf[20];
	char buffer[20]; //we need to use this buffer convert the id to string for mappers.
	int fd[2];	//creating a pipe
	for(i = 0; i < nMappers; i++)
	{
		pipe(fd);
		//spawn stream mappers
		if((s_mappers = fork()) == 0)
		{		
			dup2(fd[1], STDOUT_FILENO);
			close(fd[0]);	//cloesing stdin and stdout
			close(fd[1]);
			sprintf(stream_buf, "%d", i + 1);
			execl("./stream", "./stream", stream_buf, NULL);
			printf("Failed to create mappers");
			return 1;
		}else if (s_mappers < 0)
		{
			printf("Fork s_mappers failed");
			return 1;
		}
		//spawn mappers
		if ((mappers = fork()) == 0) 
		{	
			dup2(fd[0], STDIN_FILENO);
			close(fd[0]);	//cloesing stdin and stdout
			close(fd[1]);
			//converting id to string using sprintf and put it in buffer
			sprintf(buffer, "%d", i + 1);
			//using exec() to run mapper.c with inputs
			execl("./mapper", "./mapper", buffer, NULL);
			printf("Failed to create mappers");
			return 1;
		}
		else if (mappers < 0)
		{	//error check
			printf("Fork mappers failed");
			return 1;
		}
		//closing the pipe
		close(fd[0]);
		close(fd[1]);
	}

	// TODO: wait for all children to complete execution
	while (wait(NULL) > 0);
	
	// TODO: spawn reducers
	char buffer2[20];//we need to use this buffer convert the id to string for reducers.
	pid_t reducers;
	for (i = 0; i < nReducers; i++)
	{	//spawning mapper proccesses equal to the amount of nMappers.
		if ((reducers = fork()) == 0)
		{	//converting id to string using sprintf and put it in buffer2
			sprintf(buffer2, "%d", i + 1);
			//using exec() to run reducer.c with inputs
			execl("./reducer", "./reducer", buffer2, argv[2], NULL);
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
	while (wait(NULL) > 0);
	return EXIT_SUCCESS;
	
}


