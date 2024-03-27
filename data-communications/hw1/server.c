#include <sys.socket.h>  // socket, AF_INET, sockaddr, ..., etc
#include <arpa/inet.h>   // inet_addr, sockaddr_in, htons, htonl, INADDR_ANY	
#include <stdio.h>       // stdin, stdout, fputs
#include <string.h>      // memset
#include <unistd.h>      // read, write, close
#include <stdlib.h>      // exit, EXIT_FAILURE
#define MAXBUF 1024

void error_handling(char *msg) {
    fputs(msg, stderr);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
   int server_sockfd, sockfd, pid;
   struct sockaddr_in server_addr, client_addr;
   socklen_t client_addr_len;
   char buf[MAXBUF];

   if ((server_sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
       error_handling("socket error\n");   
   }
   server_addr.sin_family = AF_INET;
   server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
   server_addr.sin_port = htons(50000);

   if (bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
	close(server_sockfd);
	error_handling("bind error\n");
   }

   if (listen(server_sockfd, 5) < 0) {
	close(server_sockfd);
	error_handling("listen error\n");
   }
   client_addr_len = sizeof(client_addr);

   while(1)
   {
		if ((sockfd = accept(server_sockfd, (struct sockaddr *)&client_addr, &client_addr_len)) < 0) {
			close(server_sockfd);
			error_handling("accept error\n");
		}
		
		while(1) {
			memset(buf, 0x00, MAXBUF);
			if (read(sockfd, buf, MAXBUF) <= 0) {
				close(sockfd);
				error_handling("read error\n");
			}
			if (write(sockfd, buf, MAXBUF) <= 0) {
				close(sockfd);
				error_handling("write error\n");
			}
		}
		close(sockfd);
	}
	close(server_sockfd);
	return 0;
}
