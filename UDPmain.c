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
	struct sockaddr_in response[2];
	struct sockaddr_in P0Response;
	char message[256];
	fileInfoP theFileInfo;
	tokenHandlerStructP tokenHandlerInfo = 0x00;

	pthread_t threadBB, threadToken;
	pthread_attr_t attr1, attr2;
	pthread_attr_init (&attr1);
	pthread_attr_init (&attr2);
	pthread_attr_setdetachstate(&attr1, PTHREAD_CREATE_DETACHED);
	pthread_attr_setdetachstate(&attr2, PTHREAD_CREATE_DETACHED);

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
	if (getAllPeerInfo(sockfd, response, 256) < 0)
	{
		closeSocket(sockfd);
		exit(1);
	}

	P0 = amIPeerZero(sockfd, &P0Response, 256);
	
	theFileInfo = firstReadWrite(P0, sockfd);
	tokenHandlerInfo = createTokenHandlerStruct(theFileInfo, response);
	
	pthread_create(&threadBB, NULL, &bbOptions, theFileInfo);
	pthread_create(&threadToken, NULL, &handleTokenWork, tokenHandlerInfo);
	pthread_join(threadBB, NULL);
	pthread_join(threadToken, NULL);


	/*
	while(1)
	{
		readWrite(theFileInfo);
		if (theFileInfo->tokenFlag == 1)
			passToken(theFileInfo->count, response);
		else
			receiveToken(theFileInfo, &response[0]);
	}
	*/
	closeSocket(sockfd);
	return 0;
}

