#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include "UDPclient.h"
#define done 1
/*
 * A test program to start a client and connect it to a specified server.
 * Usage: client <hostname> <portnum>
 *    client is this client program
 *    <hostname> IP address or name of a host that runs the server
 *    <portnum> the numeric port number on which the server listens
 */


 /*
  * A note
  */
int main(int argc, char** argv) 
{
	int  sockfd, P0;
	struct sockaddr_in response;
	char message[256];
	char *token;
	pthread_t thread;
	pthread_attr_t attr;
	pthread_attr_init (&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	void *status;
	pthread_mutex_t count_mutex;

	if (argc != 4) {
		fprintf (stderr, "Usage: client <hostname> <portnum> <file name>\n");
		exit (1);
	}

	// parse input parameter for port information
	int portNum = atoi (argv[2]);

	// create a streaming socket
	sockfd = createSocket();
	if (sockfd < 0) {
		exit (1);
	}
	
	char *fileName = argv[4];

	// replace new line with null character
	strcpy(message,"<echo>joining ring</echo>");
	message[strlen(message)-1] = '\0';
	
	// send request to server
	if (sendRequest (sockfd, message, argv[1], portNum) < 0) {
		closeSocket (sockfd);
		exit (1);
	}

	if (receiveResponse(sockfd, &response, 256) < 0) {
		closeSocket (sockfd);
		exit (1);
	}
	
	// display response from server
	printResponse(&response);
	
	if (receiveResponse(sockfd, &response, 256) < 0) 
	{
		closeSocket (sockfd);
		exit (1);
	}
	printResponse(&response);

	int count = 1;

	P0 = amIPeerZero(sockfd, &response, 256);
	if (P0 < 0)
	{
		fprintf(stderr,"Error in peer 0 assignment\n");
		closeSocket (sockfd);
		exit (-1);
	}
	else if (P0 == 0) 
	{
		printf("I am not peer 0\n");
		int token = 0;
	}
	else 
	{
		printf("I am peer 0\n");
		int token = 1;
		createFile();
		pthread_create(&thread, NULL, &bbOptions, (void *)(intptr_t)count);
		pthread_join(thread, &status);
	}
		
	token = 0;	
	count++;
	
	//pthread_join(thread, &status);

	int errorCheck = closeSocket(sockfd);
	return 0;
}

