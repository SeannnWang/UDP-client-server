# UDP-client-server
udp client/server demo

#1/7/2023 Set all basic require functions and merge to main   
1.Exponential back off from client side  
2.create argument to config server ip ,port and max retry number

#All Functions are now in main branch!!!  

***Compile server & client by directly make in terminal which would generate executable file  
  (1)server-> server.out  
  (2)client->client.out  
  
***execute server.out with one argument for port config  
ex. ./server.out 8080  

***execute client.out by the following format  
   ./client.out "server ip address" "server port" "number of max retry"  
   where the file contains message, which contains maximum 1024 bytes,sending to server   
ex. ./client.out 127.0.0.1 8080 5
