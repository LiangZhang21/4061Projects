# Project 2 : IPC MapReduce - Word counts of different lengths

## Group Members
Test machine: virtual_box_linux_machine  
Data: 03/15/2021  
Names:ZhaoDong Wang, DeHui Zhang, Liang Zhang  
x500: wang9436, zhan6146, zhan6122  

ZhaoDong Wang  
Debugged, mapreduce.c, stream.c, mapper.c, reducer.c  
DeHui Zhang  
Comments, mapreduce.c, stream.c, mapper.c, reducer.c  
Liang Zhang  
Readme.md, mapreduce.c, stream.c, mapper.c, reducer.c  
## Purpose
The purpose of this program is to use a single machine to perform MapReduce programming model by using proccesses.  
The program use the following files: mapreduce.c, stream.c, mapper.c, reducer.c to construct the output.  
Inside the output folder, it contains an intermediate folder and a final output folder.  
<*Any hard linked files or symbolic linked files will be ignored*>  
First, the program distribute the files evently to each processes and use pipes to deilver the information from each stream mapper to mapper.  
Then the program sum up all the counts value by each proccesses from the intermediate folder. Finally, it will produce the final result in the final output folder.  
Text files from the final output folder shows the length of each words and the sum of its counts value. 
## What it does?
The followings display more details about what mapreduce.c, stream.c, mapper.c, reducer.c does.  

## Master  (mapreduce.c)
First the master will create an input directory, then use the getName() function to distribute the tasks evently for each mappers.  
<*The getName() will create a text file for each mapper and inside the text file it contains the path for the assigned text files.*>  
Then it will set up pipes form stream mappers to mappers.
The master function spawns the same amount of stream mappers and mapper processes according the input amount of nMappers.  
Each process will call the exec() function and run the stream.c and mapper.c file. The parent will wait untill the processes return before moving to the next step.  
And we repeat the same steps for reduce, where it spawns the same amount of reduce processes according the input amount of nReducers.  
Each process will call the exec() function and run the ruducer.c. And the parent will wait untill all the processes ended.  

## Stream (stream.c)
The stream.c file when it executed, it will go into the *MapperInput* folder and read into the assigned text files.  
Inside each text files, it contains the paths of the assigned tasks, and it will use the paths to extract the inforamtion and input it into the pipes.

## Map  (mapper.c)
The mapper.c file will first recevie the information form the pipes, then it will use the function parse() read the contents and put the value of the words count into an array where its indices is its words length.  
Then it will use the second function writeInterDSToFiles() to store the information in each text file at intermediate output folder.

## Reduce  (reducer.c)
The reduce.c file have two functions:  
	1)The first function reduce() read the contents from the intermediate data and sum it up to a final array.  
	2)The second function  writeFinalDSToFiles() will create and put the final words length and words count from the array into the final text file.

## Utils (utils.c)
The function we coded in the utils.c file is getReducerTasks(), this function will return an int, which is the sum of tasks. It will also distribute the tasks among each reducer and put it in an array.

## Compile
	> make clean
	> make
	
## Execution
	// always call make clean and make before you start execution
	> ./mapreduce #mappers #reducers test/T0 (or test/T1 ... so on.)

## Result
Check output folder for all the results

## Testing
You can manually check test folder and compare your results in
Final Output with expected folder.
Also, check the MapperInput folder, it should be similar to 
that in expected. It was generated for Test case 1.

## Note
For any custom test case, make sure you end the file with new line character
	

