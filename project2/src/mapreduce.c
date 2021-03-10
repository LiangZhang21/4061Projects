#include "mapreduce.h"
#include "unistd.h"

int textfile_counter = 0;
char** file_names;

int getNames(char* path){
		int file_counter;
		DIR *dir = opendir(path); //open the path directory 
		struct dirent *entry;
		if (dir == NULL){ //error check
			printf("Failed to open directory\n");
			return 1;
		}
		
		while((entry = readdir(dir)) != NULL){
			if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name,"..")) continue;
			if((entry->d_type == DT_DIR)){
				char next[strlen(path) + strlen(entry->d_name) + 2];
				next[0] = '\0';
				strcat(next, path);
				strcat(next, "/");
				strcat(next, entry->d_name);
				getNames(next);
			}else {
				char buf[200];
				buf[0] = '\0';
				strcat(buf, path);
				strcat(buf, "/");
				strcat(buf, entry->d_name);
				file_names[textfile_counter] = (char *)malloc(sizeof(buf) + strlen(entry->d_name) * sizeof(char)+1);
				strcpy(file_names[textfile_counter], buf);
				textfile_counter++;		
			}
		}
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
	for(i = 0; i < nMappers; i++){
		char stream_mapper[200] = "MapperInput/Mapper";
		sprintf(sMap_buf, "%d.txt", i + 1);
		strcat(stream_mapper, sMap_buf);
		int fd = open(stream_mapper, O_WRONLY | O_CREAT, 0777);
		if(fd == -1){
			printf("Failed to create and open the file. \n");
			exit(1);
		}
		int j;
		for(j = i; j < textfile_counter; j+= nMappers){
			strcpy(sMap_write_buf, file_names[j]);
			strcat(sMap_write_buf, "\n");
			write(fd, sMap_write_buf, sizeof(char)*strlen(file_names[j])+1);
		}
		
	}
	
	
	//TODO: spawn stream processes

	// TODO: spawn mappers	
	/*
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
	return EXIT_SUCCESS; */
}


