#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/errno.h>
#include<arpa/inet.h>
#include<netinet/in.h>

#define SRV_PORT 16384
#define MAXLEN 1024

int main(){
  int socketfd;
  socklen_t addr_length;
  int nbytes; 
  struct sockaddr_in srv_addr;
  struct sockaddr_in client_addr;
  char buff[BUFSIZ];
  
  /*Get socket descriptor*/
  socketfd = socket(AF_INET, SOCK_DGRAM, 0);
  if(socketfd<0){
    perror("Socket creation failed");
    exit(-1);
  }
  printf("Socket is successfully created\n");

  /*set up server address*/
  memset(&srv_addr, 0, sizeof(srv_addr));
  srv_addr.sin_family = AF_INET;
  srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  srv_addr.sin_port = htons(SRV_PORT);

  /*bind service address with socket descriptor*/
  if(bind(socketfd, (struct sockaddr *)&srv_addr, sizeof(srv_addr)) < 0){
    perror("bind failure");
    exit(-1);
  }
  printf("Bind Successed\nserver start listening...\n");

  /*listen to client*/
  addr_length = sizeof(client_addr);
  while(true){
    
    /*receiving client*/
    nbytes = recvfrom(socketfd, &buff, MAXLEN, 0, (struct sockaddr*)&client_addr, &addr_length);
    if(nbytes <0){
      perror("Datagram reading error\n");
      continue; 
    }      
    printf("Received %d bytes from ip address: %s and port %d\n", nbytes, inet_ntoa(client_addr.sin_addr), htons(client_addr.sin_port));
    printf("Data:%s\n", buff);

    /*return to client*/
    if(sendto(socketfd, &buff, nbytes, 0, (struct sockaddr*)&client_addr, addr_length)<0){
      perror("Datagram sending error\n");
      continue;
    }
    printf("Return message to client finished, server listening...\n");
  } 
}
