#include <stdio.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "hServer.h"

#include <time.h>
#include <ctype.h>


void *handle_client(void *socket)
{
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	time_t t1, t2;

	char buffer[BUFFER_SIZE] = {0};
	int *newsockfd_ptr = (int *)socket;
	int newsockfd = *newsockfd_ptr;
	pthread_t thread_id = pthread_self();
	printf("----------\nThread %lu using socket %x\n", (unsigned long)thread_id, newsockfd);

	/* Start communicating */
	int num_bytes = recv(newsockfd, buffer, BUFFER_SIZE - 1, 0);
	if (num_bytes < 0)
	{
		fprintf(stderr, "Thread %lu ERROR: recv() failed\n", (unsigned long)thread_id);
	}
	else
	{

		t1 = time(NULL) + (10 * 20); // server get time stamp

		for (int i = 0; i < strlen(buffer); i++)
		{
			buffer[i] = toupper(buffer[i]);
		}

		int val = strcmp(buffer, "GETTIME\n");
		if (val == 0)
		{
			num_bytes = send(newsockfd, OK_REPLY, strlen(ERROR_INVAL_MTP), 0);
			if (num_bytes < 0)
				fprintf(stderr, "Thread %lu ERROR: send() failed\n", (unsigned long)thread_id);

			char nowTime[30];
			sprintf(nowTime, "[%d-%02d-%02d %02d:%02d:%02d]\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

			printf("Thread %lu sending response\n", (unsigned long)thread_id);

			t2 = time(NULL) + (10 * 65); // server response time stamp

			printf("Sent t1: %ld\n", t1);
			num_bytes = send(newsockfd, (const void *)&t1, sizeof(time_t), 0);
			if (num_bytes < 0)
				fprintf(stderr, "Thread %lu ERROR: send() failed\n", (unsigned long)thread_id);

			printf("Sent t2: %ld\n", t2);
			num_bytes = send(newsockfd, (const void *)&t2, sizeof(time_t), 0);
			if (num_bytes < 0)
				fprintf(stderr, "Thread %lu ERROR: send() failed\n", (unsigned long)thread_id);
		}
		else
		{
			num_bytes = send(newsockfd, ERROR_INVAL_MTP, strlen(ERROR_INVAL_MTP), 0);
			if (num_bytes < 0)
				fprintf(stderr, "Thread %lu ERROR: send() failed\n", (unsigned long)thread_id);
			printf("ERROR: Invalid MTP Request\n");
		}
	}

	close(newsockfd);
	return NULL;
}

int main(int argc, char *argv[])
{
	struct sockaddr_in serv_addr = {0}, cli_addr;
	socklen_t clilen = sizeof(cli_addr);
	int sockfd, thread_res;
	pthread_t mythread;

	/* Create a socket */
	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockfd < 0)
	{
		fprintf(stderr, "ERROR: Failed to open socket\n");
		return 1;
	}

	/* allow to reuse the socket as soon as it stops being active */
	int so_reuse_enabled = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &so_reuse_enabled, sizeof(int)) < 0)
	{
		fprintf(stderr, "setsockopt(SO_REUSEADDR) failed");
		return -2;
	}

	/* Initialize socket structure (sockarrd_in) */
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // or INADDR_ANY
	serv_addr.sin_port = htons(PORT);

	/* Bind the host address */
	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		fprintf(stderr, "ERROR: bind() failed\n");
		return 2;
	}

	/* Start listening for the clients (thread blocks) */
	if (listen(sockfd, MAX_CONNECTIONS) != 0)
	{
		fprintf(stderr, "ERROR: listen() failed\n");
		return 3;
	}

	for (;;)
	{
		/* Accept connection from a client */
		int newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
		if (newsockfd < 0)
		{
			fprintf(stderr, "ERROR: accept() failed\n");
			return 4;
		}

		thread_res = pthread_create(&mythread, NULL, handle_client, &newsockfd);
		if (thread_res != 0)
		{
			fprintf(stderr, "ERROR: pthread_create() failed\n");
			return 5;
		}
	}

	return 0;
}