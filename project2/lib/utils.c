#include "utils.h"

void writeLineToFile(char *filepath, char *line) {
    int fd = open(filepath, O_CREAT | O_WRONLY, 0777);
    if (fd < 0){
        printf("ERROR: Cannot open the file %s\n", filepath);
        fflush(stdout);
        exit(EXIT_FAILURE);
    }
    int ret = write(fd, line, strlen(line));
    if(ret < 0){
        printf("ERROR: Cannot write to file %s\n", filepath);
        fflush(stdout);
        exit(EXIT_FAILURE);
    }
}

FILE * getFilePointer(char *inputFileName) {
    return fopen(inputFileName, "r");
}

ssize_t getLineFromFile(FILE *fp, char *line, size_t len) {
    memset(line, '\0', len);
    return getline(&line, &len, fp);
}


//TODO
// return: The number of intermediate files the reducer should process
// The list of intermediate file names are stored in myTasks array
int getReducerTasks(int nReducers, int reducerID, char *intermediateDir, char **myTasks) {
	int i;
	int counter = 1;
	int sum = 0;
	for(i = reducerID; i <= 20; i += nReducers){
		char folder_buf[400];
		sprintf(folder_buf, "%s/%d", intermediateDir, i);
		DIR *dir = opendir(folder_buf);
		struct dirent *entry;
		if (dir == NULL){ //error check
			printf("Failed to open directory\n");
			return 1;
		}
		while((entry = readdir(dir)) != NULL){
			if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) continue;
			char *file_buf = malloc(sizeof(char) * 2000);
			sprintf(file_buf, "%s/m%d.txt", folder_buf, counter);
			myTasks[sum] = file_buf;	
			sum++;
			counter++;
		}
		counter = 0;
	}
	return sum;
}


//TODO: traverse inputfile directory and create MapperInput directory
//We wrote our own function in mapreduce.c
void traverseFS(int mappers, char *path){

}

int isValidDir(char *folder) {
    struct stat sb;
    return (stat(folder, &sb) == 0 && S_ISDIR(sb.st_mode));
}

void _removeOutputDir(){
    pid_t pid = fork();
    if(pid == 0){
        char *argv[] = {"rm", "-rf", "output", NULL};
        if (execvp(*argv, argv) < 0) {
            printf("ERROR: exec failed\n");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    } else{
        wait(NULL);
    }
}

void _createOutputDir(){
    mkdir("output", ACCESSPERMS);
    mkdir("output/IntermediateData", ACCESSPERMS);
    mkdir("output/FinalData", ACCESSPERMS);
}

void _createInterFolders(){
    int wordLen;
    for(wordLen = 1; wordLen <= MaxWordLength; wordLen++) {
        char dirName[50];
        memset(dirName, '\0', 50);
        sprintf(dirName, "output/IntermediateData/%d", wordLen);
        mkdir(dirName, ACCESSPERMS);
    }
}

void bookeepingCode(){
    _removeOutputDir();
    sleep(1);
    _createOutputDir();
    _createInterFolders();
}



