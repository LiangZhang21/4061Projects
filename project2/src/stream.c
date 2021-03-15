#include "stream.h"

/**
 * read lines from input file and write each line to pipes
 * each line will contain words as in Project 1 (Use STDOUT for writing)
 */
//This function does what the requirement requested above.
void emit(char * inputFileName) {
	char buffer [200];
	FILE *fd2 = getFilePointer(inputFileName);	//open up the file
	while(getLineFromFile(fd2, buffer, 200) > 0){
		write(STDOUT_FILENO, buffer, 200);	//writing into the pipe
	}
}

/***
 *
 * Stream process will read from the files created by Master
 */
int main(int argc, char *argv[]) {

    mapperID = strtol(argv[1], NULL, 10);
    char* ipFdr = "MapperInput/Mapper";
	
	int i;
	char pathH_buf[20];
	char path_holder[200];
    //TODO: Read lines from Mapper files and get the file names that will be processes in emit function
  	strcat(path_holder, ipFdr);
  	sprintf(pathH_buf, "%d.txt", mapperID);
  	strcat(path_holder, pathH_buf);
	FILE *fp = getFilePointer(path_holder); 
  
    //Each MapperInput/MapperID.txt file will contain file names
    // example of one line will be test/T1/subfolder/0.txt
    char filePath[maxFileNameLength];
 	int temp;
 	//Using the while loop to extract each line from the file and call emit()
    while((temp = getLineFromFile(fp, filePath, maxFileNameLength)) > 0){
    	filePath[temp-1] = '\0';
        emit(filePath);
    }
   	
    return EXIT_SUCCESS;
}
