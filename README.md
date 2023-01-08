# UDP-client-server
udp client/server demo

#1/7/2023 Add new functions!  
1.Exponential back off from client side  
2.create argument to config server ip ,port and max retry number

***Compile server & client by directly make in terminal which would generate executable file  
  (1)server-> server.out  
  (2)client->client.out 
***execute server.out directly  
***execute client.out by the following format ./client.out <server ip address> <server port> <number of max retry>
   where the file contains message, which contains maximum 1024 bytes,sending to server 

#1/7/2023 first version
~~1.Compile server & client by directly make in terminal which would generate executable file  ~~
  ~~(1)server-> server.out  ~~
  ~~(2)client->client.out  ~~
    
~~2.execute server.out directly  ~~
~~3.execute client.out by the following format ./client.out <127.0.0.1> <filename>  ~~
  ~~where the file contains message, which contains maximum 1024 bytes,sending to server  ~~
