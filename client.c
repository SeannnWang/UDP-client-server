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
  socklen_t addr_length;  
  char read_data[MAXLEN+1];
  char srv_response[MAXLEN+1];
  //server ip and port related vars
  in_addr_t host_ip;
  int srv_port;
  struct sockaddr_in client_addr;
  struct sockaddr_in srv_addr;
  
  //variables which handle time out
  fd_set timeout_read_fds;
  int ret;
  
  //Max retry
  int max_retry = atoi(argv[3]);
    
  /*
   *Check input format
   */
  if(argc<3){
    printf("invalid client side usage\nclient format is <server ip address> <server port> <max num of retry>\n");
    exit(-1);
  }

  /*
   *create socket
   */
  socketfd = socket(AF_INET, SOCK_DGRAM, 0);
  if(socketfd<0){
    perror("socket creation failed\n");
    exit(-1);
  }
  
  /*
   *Bind client address with socket descriptor
   */
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
  //handle ip address from user input
  host_ip = inet_addr(argv[1]);
  if(host_ip == INADDR_NONE){
    printf("cannot read server address from %s\n", argv[1]);
    printf("client format is <server ip address> <server port> <max num of retry> <message>\n");
    exit(-1);
  } 
  
  //handle port from user input
  srv_port = atoi(argv[2]);
  if(srv_port==0){
    printf("Server port is not in valid format\n");
    printf("client format is <server ip address> <server port> <max num of retry> <message>\n");
    exit(-1);
  }
  
  memset(&srv_addr, 0, sizeof(srv_addr));
  srv_addr.sin_family = AF_INET;
  srv_addr.sin_addr.s_addr = host_ip;
  srv_addr.sin_port = htons(srv_port);    

  /*
   * Set up Time out handler
   */
  int retry_num = 0; 
  struct timeval timeout;
  timeout.tv_sec = RETRYBASE*pow(MULTIPLIER,retry_num);
  timeout.tv_usec = 0;    

  /*
   *send message to server
   */
  addr_length = sizeof(srv_addr);
  printf("Enter message: ");
  char chat[MAXLEN];
  fgets(chat, 1024, stdin); 
  
  while(1){    
    memcpy(read_data, chat, strlen(chat)+1);       
    if(sendto(socketfd, read_data, sizeof(read_data), 0, (struct sockaddr*)&srv_addr, addr_length)<0){
      perror("Datagram sending error\n");
      exit(-1);
    }
    
    /*
     *set time out check descriptor
     */
    FD_ZERO(&timeout_read_fds);
    FD_SET(socketfd, &timeout_read_fds);
 
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
      printf("Timeout in %.3f sec, ", RETRYBASE*pow(MULTIPLIER,retry_num)); 
      
      if(++retry_num > max_retry){
        printf("exceed max retry:%d, exit program with code(1)\n", max_retry);
        exit(1);
      }
      if(RETRYBASE*pow(MULTIPLIER,retry_num) > MAXINTERVAL){
        printf("\nExceed MAX WAIT INTERVAL:%d, exit program with code(1)\n", MAXINTERVAL);
        exit(1);
      }
      printf("trigger retry mechanism\n\nRetry: %d times:\n", retry_num);         
      timeout.tv_sec = RETRYBASE*pow(MULTIPLIER,retry_num);
      continue;
    }   
    else{
      //data is available on socket
      if(recvfrom(socketfd, srv_response, MAXLEN, 0, (struct sockaddr*)&srv_addr, &addr_length)<0){
        perror("Datagram reading error\n");
        exit(-1);
      }      
        printf("Get server response: %s\n", srv_response);
        exit(0);
    }  
  }         
}
