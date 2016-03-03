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
int receiveResponse(int sockFd, struct sockaddr_in *, int size);
void printResponse(struct sockaddr_in *);
int amIPeerZero(int sockFd, struct sockaddr_in *, int size);
fileInfoP firstReadWrite(int P0, int sockfd, int count);


 /*
  * A note
  */
int main(int argc, char** argv) 
{
	int  sockfd, P0;
	struct sockaddr_in response;
	char message[256];

	if (argc != 4) 
	{
		fprintf (stderr, "Usage: client <hostname> <portnum> <file name>\n");
		exit (1);
	}

	// parse input parameter for port information
	int portNum = atoi (argv[2]);

	// create a streaming socket
	sockfd = createSocket();
	if (sockfd < 0) 
	{
		exit (1);
	}
	
	// replace new line with null character
	strcpy(message,"<echo>joining ring</echo>");
	message[strlen(message)-1] = '\0';
	
	// send request to server
	if (sendRequest (sockfd, message, argv[1], portNum) < 0) 
	{
		closeSocket (sockfd);
		exit (1);
	}

	if (receiveResponse(sockfd, &response, 256) < 0) 
	{
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
	firstReadWrite(P0, sockfd, count);
	
	closeSocket(sockfd);
	return 0;
}

