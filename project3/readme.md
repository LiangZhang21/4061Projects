 Project 3 :

## Group Members
Test machine: virtual_box_linux_machine  
Data: 03/2/2021  
Names:ZhaoDong Wang, DeHui Zhang, Liang Zhang  
x500: wang9436, zhan6146, zhan6122  

ZhaoDong Wang  
Debugged, error checks, consumer.c, main.c, producer.c  
DeHui Zhang  
consumer.c, main.c, producer.c  
Liang Zhang  
Readme.md, comment, consumer.c, main.c, producer.c  
## Purpose
The purpose of this program is to use a single machine to perform MapReduce programming model by using multi-threads.  
The program use the following files: consumer.c, main.c, producer.c to construct a single output file.  
First, the program will use a single producer to read each line of the file and produce a linked-list.  
Then the consumers will take the data from the linked-list and drop the data into the array where its index is the words' length and its value is the count.
Finally, the program will produce the final result using the array in the final output folder.  
## What it does?
The followings display more details about what consumer.c, main.c, producer.c does.  

## Master  (main.c)
  The main.c file create a producer thread and consumer threads; the producer thread will call the producer.c  
functions to read in the count of the word length and the consumers takes that data and call the  
 writeFinalDSToFiles() output the data into a final text file.

## producer.c
	The producer.c will read in each line of the file and store it into a linked-list.  
The critial section of the function uses mutex lock and conditional variable to prevent race condition.  
On "-p" or "-bp" command, it will also print out a log of the items it has produced.  

## consumer.c
	The consumer.c will read in data from the linked-list and split the line into words.  
Then, it will store the data into an array (where its index is the words' length and its value is the  
count). The critial section of the function also uses mutex lock and conditional variable to prevent race  
condition. On "-p" or "-bp" command, it will also print out a log of the items it has consumed.  


## Compile
	> make clean
	> make
	//optinal argument has three options: "-p", "-b", "-bp"
		- "-p" will generate log
		- "-b" allow bounded buffer, else the program will use unbounded buffer instead.
		- "-bp" allows for both options
	> ./mapreduce #consumers inputFile [option] [#queueSize]
	
