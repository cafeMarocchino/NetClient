/*
Student ID : 20133230
Name : 성정훈
*/
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 100

void error_handling(char *message);
void display();
int num_socket = 0;

#define MAXCLIENT (16)
int clienttcpSocket[MAXCLIENT];	// max 16 clients


int main(int argc, char **argv) {
	
  int tcpServ_sock, new_sock;
  int addrlen; 
  struct sockaddr_in tcpServer_addr;
  struct sockaddr_in tcpClient_addr;
  struct sockaddr_in newTcp_addr;

  // initialize the client socket descriptors
  for(int i = 0; i < MAXCLIENT; i++) {
  	clienttcpSocket[i] = -1;;	// max 128 clients
    
  }

  int slot;
  socklen_t clnt_len;
    
  fd_set reads, temps;
  int fd_max;

  
  char str[BUFSIZE];
  int option = 2;

  char *tcpport = NULL;

  if(argc != 2) {
    printf("Usage : %s <tcpport> \n", argv[0]);
    exit(1);
  }

  tcpport = argv[1];

  display();
	
  tcpServ_sock = socket(PF_INET, SOCK_STREAM, 0);
  if(tcpServ_sock == -1)
	  error_handling("socket() error");
  
  memset(&tcpServer_addr, 0 , sizeof(tcpServer_addr));
  tcpServer_addr.sin_family = AF_INET;
  tcpServer_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  tcpServer_addr.sin_port = htons(atoi(tcpport));

  setsockopt(tcpServ_sock, SOL_SOCKET, SO_REUSEADDR, (const void *)&option, sizeof(int));
  if( bind(tcpServ_sock, (struct sockaddr *) &tcpServer_addr, sizeof(tcpServer_addr)) == -1 )
	  error_handling("bind() error");

			
  if(listen(tcpServ_sock, 5)==-1)
    error_handling("listen() error");
  
  FD_ZERO(&reads);
  FD_SET(tcpServ_sock, &reads);
  fd_max = tcpServ_sock;

  while(1) {
    int nfound = 0;
    temps = reads;
    nfound = select(fd_max+1, &temps, 0, 0, NULL);
	  if(FD_ISSET(tcpServ_sock, &temps)) {
      addrlen = sizeof(tcpClient_addr);
      clienttcpSocket[num_socket++] = accept(tcpServ_sock,
      (struct sockaddr *)&tcpClient_addr, &addrlen);
      if (clienttcpSocket[num_socket - 1] < 0) {
        perror("accept");
        exit(1);
      }
      int i;
      printf("connection from host %s, port %d, socket %d\n",
        inet_ntoa(tcpClient_addr.sin_addr), ntohs(tcpClient_addr.sin_port), clienttcpSocket[num_socket - 1]);
      FD_SET(clienttcpSocket[num_socket - 1], &reads);
      fd_max++;
      FD_CLR(tcpServ_sock, &temps);
	  } else {
      for(int i = 0; i < MAXCLIENT; i++) {
        if (FD_ISSET(clienttcpSocket[i], &temps)) {
          int bytesread = read(clienttcpSocket[i], str, sizeof str - 1);
          if (bytesread < 0) {
            perror("read");
          } else if(bytesread  <= 0) {
            printf("Connetion Closed  %d\n",clienttcpSocket[i]);
            FD_CLR(clienttcpSocket[i], &reads);
            close(clienttcpSocket[i]);
            for(int m = i + 1; m < MAXCLIENT; m++)
              clienttcpSocket[m - 1] = clienttcpSocket[m];
            num_socket--;
          } else {
            str[bytesread] = '\0';
            printf("%d: %s", clienttcpSocket[i], str);
            for(int j = 0; j < MAXCLIENT; j++) {
              if(clienttcpSocket[j] != -1 && i != j) {
                if (write(clienttcpSocket[j], str, bytesread)!=bytesread)
                  perror("echo");
              }
            }
          }
        }
		  }
	  }
  }//while End
}//main End

void display() {
	printf("Student ID : 20133230 \n");
	printf("Name : 성정훈  \n");
}

void error_handling(char *message) {
  fputs(message, stderr);
  fputc('\n', stderr);
  perror("hw2 error");
  exit(1);
}

