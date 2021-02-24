# Project 1 : MapReduce - Word counts of different lengths
## Group Members
test machine: virtual_box_linux_machine  
data: 02/21/2021  
name:ZhaoDong Wang, DeHui Zhang, Liang Zhang  
x500: wang9436,  zhan6146,  zhan6122  
 
ZhaoDong Wang  
Debugged, worked on mapreduce and mapper.c  
DeHui Zhang  
Wrote comments, debugged, and worked on reduce.c  
Liang Zhang  
Wrote readme.md, worked on mapreduce and mapper.c  
## Purpose
The purpose of this program is to use a single machine to perform MapReduce programming model by using proccesses.  
It takes in a list of files and use the following functions: mapreduce.c, mapper.c, reducer.c to construct the output folder.  
And in the  output, it contains an intermediate folder and a final output folder.  
The program then sum up all the counts value by each proccesses from the intermediate folder and produce the final result in the final output folder.  
Text files from the final output folder shows the length of each words and the sum of its counts value. 
## What it does?
The following information display more details about what mapreduce.c, mapper.c, reducer.c does.
## Master function (mapreduce.c)
The master function spawns the same amount of mapper processes according the input amount of nMappers.  
Each process will call the exec() function and run the mapper.c file. The parent will wait untill the processes return before moving to the next step.  
And we repeat the same steps for reduce, where it spawns the same amount of reduce processes according the input amount of nReducers.  
Each process will call the exec() function and run the ruducer.c. And the parent will wait untill all the processes ended.  
## Map function (mapper.c)
The mapper.c file have two functions:  
	1)The first function map() read the contents of the file, and put the value of the words count into an array where its indices is its words length.  
	2)The second function writeInterDSToFiles() will create and put the words length and words count  from the array into the text file.

## Reduce function (reducer.c)
The reduce.c file have two functions:  
	1)The first function reduce() read the contents from the intermediate data and sum it up to a final array.  
	2)The second function  writeFinalDSToFiles() will create and put the final words length and words count from the array into the final text file.

## Compile
	> make clean
	> make
	> make t0
	> make t1 ... so on  
	use make to run the file you desire and it will produce the result in the output folder.

## Execution
	// always call make clean and make before you start execution
	// ./mapreduce nMappers nReducer inputFileDir
	> ./mapreduce 5 3 test/T0

## Result
Check output folder for all the results
	
## Note:
The current code is the first draft. There might be some edge case that I missed, like the empty file case. I am working on them. The code is just for giving you an idea of how the overall project looks like.

The header files are not required as the function definition is in the same file as usage. This is the layout that I usually follow.

