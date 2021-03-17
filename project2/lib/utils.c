#include "utils.h"

void writeLineToFile(char *filepath, char *line)
{
    int fd = open(filepath, O_CREAT | O_WRONLY, 0777);
    if (fd < 0)
    {
        printf("ERROR: Cannot open the file %s\n", filepath);
        fflush(stdout);
        exit(EXIT_FAILURE);
    }
    int ret = write(fd, line, strlen(line));
    if (ret < 0)
    {
        printf("ERROR: Cannot write to file %s\n", filepath);
        fflush(stdout);
        exit(EXIT_FAILURE);
    }
}

FILE *getFilePointer(char *inputFileName)
{
    return fopen(inputFileName, "r");
}

ssize_t getLineFromFile(FILE *fp, char *line, size_t len)
{
    memset(line, '\0', len);
    return getline(&line, &len, fp);
}

//TODO
// return: The number of intermediate files the reducer should process
// The list of intermediate file names are stored in myTasks array
int getReducerTasks(int nReducers, int reducerID, char *intermediateDir, char **myTasks)
{
    int i;
    int counter = 1; //use counter for file names in each folder
    int sum = 0;     //use total sum for return
    for (i = reducerID; i <= 20; i += nReducers)
    {
        char folder_buf[maxFileNameLength];
        sprintf(folder_buf, "%s/%d", intermediateDir, i);
        //opening up the folder
        DIR *dir = opendir(folder_buf);
        struct dirent *entry;
        if (dir == NULL)
        { //error checking
            fprintf(stderr, "Failed to open directory\n");
            return EXIT_FAILURE;
        }
        //while inside the folder it is not empty
        while ((entry = readdir(dir)) != NULL)
        {
            //skipping "." and ".."
            if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
                continue;
            char *file_buf = malloc(sizeof(char) * maxFileNameLength);
            sprintf(file_buf, "%s/m%d.txt", folder_buf, counter);
            //adding the path to the array.
            myTasks[sum] = file_buf;
            //increments the sum and counter
            sum++;
            counter++;
        }
        counter = 1;
        if (closedir(dir))
        {
            fprintf(stderr, "ERROR: closing directory failed\n");
            return EXIT_FAILURE;
        }
    }

    return sum;
}

//TODO: traverse inputfile directory and create MapperInput directory
//We wrote our own function in mapreduce.c
void traverseFS(int mappers, char *path)
{
}

int isValidDir(char *folder)
{
    struct stat sb;
    return (stat(folder, &sb) == 0 && S_ISDIR(sb.st_mode));
}

void _removeOutputDir()
{
    pid_t pid = fork();
    if (pid == 0)
    {
        char *argv[] = {"rm", "-rf", "output", NULL};
        if (execvp(*argv, argv) < 0)
        {
            printf("ERROR: exec failed\n");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }
    else
    {
        wait(NULL);
    }
}

void _createOutputDir()
{
    mkdir("output", ACCESSPERMS);
    mkdir("output/IntermediateData", ACCESSPERMS);
    mkdir("output/FinalData", ACCESSPERMS);
}

void _createInterFolders()
{
    int wordLen;
    for (wordLen = 1; wordLen <= MaxWordLength; wordLen++)
    {
        char dirName[50];
        memset(dirName, '\0', 50);
        sprintf(dirName, "output/IntermediateData/%d", wordLen);
        mkdir(dirName, ACCESSPERMS);
    }
}

void bookeepingCode()
{
    _removeOutputDir();
    sleep(1);
    _createOutputDir();
    _createInterFolders();
}
