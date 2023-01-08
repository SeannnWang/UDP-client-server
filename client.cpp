#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/errno.h>
#include<sys/time.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<fcntl.h>
#include<netdb.h>
#include<signal.h>
#include<math.h>

#define SRV_PORT 16384
#define MAXLEN 1024
#define MULTIPLIER 2
#define RETRYBASE 0.5
#define MAXINTERVAL 8

int main(int argc, char **argv){
  int socketfd;
  int msg_length;
  socklen_t addr_length;
  int input_fd;
  int nbyte;
  char read_data[MAXLEN+1];
  char srv_response[MAXLEN+1];
  in_addr_t host_ip;
  struct sockaddr_in client_addr;
  struct sockaddr_in srv_addr;
  /*variables which handle time out*/
  fd_set timeout_read_fds;
  int ret;
  /*Max retry*/
  int max_retry = 3;
    
  /*Check input parameter*/
  if(argc<3){
    printf("invalid client side usage, check client format and retry\n");
    exit(-1);
  }

  /*create socket*/
  socketfd = socket(AF_INET, SOCK_DGRAM, 0);
  if(socketfd<0){
    perror("socket creation failed\n");
    exit(-1);
  }
  
  /*Bind client address with socket descriptor*/
  memset(&client_addr, 0, sizeof(client_addr));
  client_addr.sin_family = AF_INET;
  client_addr.sin_addr.s_addr = (INADDR_ANY);
  client_addr.sin_port = htons(0);
  
  if(bind(socketfd, (struct sockaddr*)&client_addr, sizeof(client_addr))<0){
    perror("Bind failure\n");
    exit(-1);
  }
  
  /*
   *Bind server IP address
   */  
  /*handle ip address from user input*/
  host_ip = inet_addr(argv[1]);
  if(host_ip == INADDR_NONE){
    printf("cannot read server address from %s\n", argv[1]);
    exit(-1);
  } 
  
  memset(&srv_addr, 0, sizeof(srv_addr));
  srv_addr.sin_family = AF_INET;
  client_addr.sin_addr.s_addr = host_ip;
  srv_addr.sin_port = htons(SRV_PORT);
  
  /*read input from file*/
  input_fd = open(argv[2], O_RDONLY);
  if(input_fd==-1){
    printf("file open failed\n");
    exit(-1);
  }

  nbyte = read(input_fd, read_data, MAXLEN);
  if(nbyte<0){
    printf("file read falied\n");
    exit(-1);
  }
  if(nbyte>MAXLEN){
    printf("Message exceed max length\n");   
    exit(-1);
  }    

  /*
   * Set up Time out handler
   */
  int retry_num = 0; 
  struct timeval timeout;
  timeout.tv_sec = RETRYBASE*pow(MULTIPLIER,retry_num);
  timeout.tv_usec = 0;
  
  //set time out check descriptor
  FD_ZERO(&timeout_read_fds);
  FD_SET(socketfd, &timeout_read_fds);


  /*
   *send message to server
   */
  addr_length = sizeof(srv_addr);

  while(true){
    if(sendto(socketfd, read_data, nbyte, 0, (struct sockaddr*)&srv_addr, addr_length)<0){
      perror("Datagram sending error\n");
      exit(-1);
    }
 
    /*
     *receive message from server
     */  
    ret = select(socketfd+1, &timeout_read_fds, NULL, NULL, &timeout);

    if(ret<0){
      perror("select failed");
      exit(-1);
    }
    else if(ret==0){
      //timeout occurred, implement exponential backoff
      double interval = RETRYBASE*pow(MULTIPLIER,retry_num);      
      printf("Timeout in %.3f sec, ", interval); 
      
      if(++retry_num > max_retry){
        printf("exceed max retry:%d, exit program with code '1'", max_retry);
        exit(1);
      }
      if(interval > MAXINTERVAL){
        printf("\nExceed MAXINTERVAL:%d, exit program with code(1)\n", MAXINTERVAL);
        exit(1);
      }
      printf("trigger retry mechanism\n\nRetry: %d times:\n", retry_num);         
      timeout.tv_sec = RETRYBASE*pow(MULTIPLIER,retry_num);
      continue;
    }   
    else{
      //data is available on socket
      if(recvfrom(socketfd, srv_response, MAXLEN, 0, (struct sockaddr*)&srv_addr,&addr_length)<0){
        perror("Datagram reading error\n");
        exit(-1);
      }      
        printf("server response: %s", srv_response);
        exit(0);
    }  
  }         
}
