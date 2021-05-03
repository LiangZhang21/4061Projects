Project 4 :

## Group Members
Test machine: virtual_box_linux_machine  
Data: 05/01/2021  
Names:ZhaoDong Wang, DeHui Zhang, Liang Zhang  
x500: wang9436, zhan6146, zhan6122

ZhaoDong Wang  
Debugged, error checks, client.c , server.c, extra credits  
DeHui Zhang  
client.c , server.c  
Liang Zhang  
Readme.md, comment, client.c , server.c, extra credits  

## Purpose
The purpose of this program is to use a single machine to perform MapReduce programming model by using server client method.  
First, each clients will perform a connection to a server. 
Then program will use clients to read each of the files in a roundabout fashion and send the counts to the server.  
Then the server will take the data from different clients and store the data into a global array where its index is the words' length and its value is the count.  
Finally, the sever will return the result of each request back to the clients, and the clients will output the results in the log file.  

*Extra credits is compeleted*  

## What it does?
The followings display more details about what client.c , server.c does.  


## client.c
	Each clients will read each of the files in a roundabout fashion and store the data into a array and send the request to the server.  
(Each clients are different proccesses)  
After the server return the data back to the clients through a socket, it will output the data into a log file.  

## server.c
	The server will accepts the request from each clients and send back the data back to each clients  
The server create a thread for each client to handle the requests, upon receving the request it will send the requested data back to the clients through a socket between the client and server.  


## Compile
	//Server Side
	> make clean (make clean is required for each compile)
	> make
	> ./server [port_#]
	
	//Client Side
	> make clean (make clean is required for each compile)
	> make
	> ./client [folder_path] [client_#] [IP_address] [port_#]
	
