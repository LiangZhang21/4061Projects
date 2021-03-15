#include "mapper.h"

/**
 * Write word count that is stored in an intermediate data structure to files.
 * The file should be in a corresponding folder in output/IntermediateData/ 
 */
void writeInterDSToFiles(void) {
	int i;
	char buf[30];
	char buf2[maxFileNameLength];
	for(i = 0; i < MaxWordLength; i++){
		sprintf(buf, "%d %d", i + 1, interDS[i]);
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
void parse(char * line) {
	char buffer[200];
	buffer[0] = '\0';
	strcat(buffer, line);
	char *token = strtok(buffer, "\n");
	token = strtok(token, " ");
	while(token != NULL){
		interDS[strlen(token) - 1]++;
		token = strtok(NULL, " ");
	}
}

int main(int argc, char *argv[]) {

    mapperID = strtol(argv[1], NULL, 10);

    // you can read lines from pipes (from STDIN) (read lines in a while loop)
    //feel free to change
    char buf[200];
    while(read(STDIN_FILENO, buf, 200) > 0) {
        parse(buf);
    }

    writeInterDSToFiles();

    return EXIT_SUCCESS;
}
