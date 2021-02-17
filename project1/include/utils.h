#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <ctype.h>

#define chunkSize 1024
#define MaxNumProcesses 20
#define MaxWordLength 20
#define maxFileNameLength 200
#define MaxNumInputFiles 100
#define MaxNumIntermediateFiles 400

// mapper side
int getMapperTasks(int nMappers, int mapperID, char *inputFileDir, char **myTasks);

// reducer side
int getReducerTasks(int nReducers, int reducerID, char *intermediateDir, char **myTasks);

// file I/O
/**
 * Get a pointer to a opened file based on the file name
 * @param *inputFileName  the file path
 * @return a file pointer pointing to the file
 */
FILE * getFilePointer(char *inputFileName);

/**
 * Read an entire line from a file
 * @param  *fp    the file to be read
 * @param  *line  contain the line content
 * @param  len    the size of the line
 * @return the number of character reads (including the newline \n, but not including terminator)
           -1 when reaching the end of file or error occurs
 */
ssize_t getLineFromFile(FILE *fp, char *line, size_t len);

/**
 * Open a file, and write a line to the file
 * @param *filepath  the file path
 * @param *line      the line content
 */
void writeLineToFile(char *filepath, char *line);

// directory
void bookeepingCode();
int isValidDir(char *folder);

#endif