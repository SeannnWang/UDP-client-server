# UDP-client-server
udp client/server demo

#1/7/2023 Set all basic require functions!  
1.Exponential back off from client side  
2.create argument to config server ip ,port and max retry number

***Compile server & client by directly make in terminal which would generate executable file  
  (1)server-> server.out  
  (2)client->client.out 
***execute server.out directly  
***execute client.out by the following format ./client.out <server ip address> <server port> <number of max retry>  
   where the file contains message, which contains maximum 1024 bytes,sending to server 
