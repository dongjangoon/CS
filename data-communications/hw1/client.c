#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define MAXBUF 1024
#define QUEUE_SIZE 100

// define queue struct
typedef struct {
	char messages[QUEUE_SIZE][MAXBUF];
	int front, rear;
} MessageQueue;

void initQueue(MessageQueue *q) {
	q->front = q->rear = 0;
}

void isQueueEmpty(MessageQueue *q) {
	return q->front == q->rear;
}

void enqueue(MessageQueue *q, char *message) {
	strcpy(q->messages[q->rear], message);
	q->rear = (q->rear + 1) % QUEUE_SIZE;
}

void dequeue(MessageQueue *q, char *message) {
	strcpy(message, q->messages[q->front]);
	q->front = (q->front + 1) % QUEUE_SIZE;
}

void error_handling(char *msg) {
	fputs(msg, stderr);
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
	struct sockaddr_in server_addr;
	int sockfd;
	socklen_t server_addr_len;
	char buf[MAXBUF];

	MessageQueue messageQueue;
	initQueue(&messageQueue);

	if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		error_handling("socket error\n");
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	server_addr.sin_port = htons(50000);
	server_addr_len = sizeof(server_addr);

	if (connect(sockfd, (struct sockaddr *)&server_addr, server_addr_len) < 0) {
		close(sockfd);
		error_handling("connect error\n");
	}

	while (1) {

		memset(buf, 0x00, MAXBUF);
		fgets(buf, MAXBUF, stdin);

		// When input is 'Q\n', send queued messages
		if (strcmp(buf, "Q\n") == 0) {
			printf("SEND\n");
			printf("%s\n", buf);
			printf("RECV\n");
		}

		// When input is 'bye\n', send 'Echo_CLOSE' and close the connection
		if (strcmp(buf, "bye\n") == 0) {
			printf("Echo_CLOSE\n");
			break
		}

		enqueue(&messageQueue, buf);

		if (write(sockfd, buf, MAXBUF) <= 0) {
			close(sockfd);
			error_handling("write error\n");
		}

		if (read(sockfd, buf, MAXBUF) <= 0) {
			close(sockfd);
			error_handling("read error\n");
		}
	}

	close(sockfd);
	return 0;
}
