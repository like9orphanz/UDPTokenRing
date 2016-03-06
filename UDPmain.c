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
	tokenHandlerStructP tokenHandler = 0x00;

	pthread_mutex_t mainLock;
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
	printf("commit reverted\n");
	P0 = amIPeerZero(sockfd, &P0Response, 256);
	theFileInfo = firstReadWrite(P0, sockfd);
	tokenHandler = createTokenHandlerStruct(theFileInfo, response);
	tokenHandler->sock = sockfd;
	pthread_create(&threadBB, NULL, &bbOptions, theFileInfo);
	printf("theFileInfo->tokenFlag = %d\n",theFileInfo->tokenFlag);
	int a = 0;
	while (1) 
	{
		if (theFileInfo->tokenFlag == 0)
		{
			printf("if\n");
			if (receiveToken(tokenHandler->sock, tokenHandler->theFileInfo, tokenHandler->neighborInfo) < 0)
			{
				fprintf(stderr, "error in receiving token\n");
				exit (-1);
			}

		}
		else
		{
			printf("else\n");
			while (!a)
			{	
				pthread_mutex_lock(&mainLock);
				if (theFileInfo->tokenFlag == 0)
				{	
					printf("I went from 1 to 0, triggering passToken yo momma\n");
					if (passToken(tokenHandler->sock, tokenHandler->theFileInfo, tokenHandler->neighborInfo) < 0)
					{
						fprintf(stderr, "error in passing token\n");
						exit (-1);
					}
					a = 1;
				}
				pthread_mutex_unlock(&mainLock);
			}
			a = 0;
		}
	}
	// pass token if peer zero
	/*if (P0 == 1)
	{
		if (passToken(tokenHandler->sock, tokenHandler->theFileInfo, tokenHandler->neighborInfo) < 0)
		{
			fprintf(stderr, "error in passing token\n");
			exit (-1);
		}
	}
	else
	{
		if (receiveToken(tokenHandler->sock, tokenHandler->theFileInfo, tokenHandler->neighborInfo) < 0)
		{
			fprintf(stderr, "error in receiving token\n");
			exit (-1);
		}
	}
	*/
	/*
	int i = 0;
	while (i < 3)
	{
		if (tokenHandler->theFileInfo->tokenFlag == 1)
		{
			if (passToken(tokenHandler->sock, tokenHandler->theFileInfo, tokenHandler->neighborInfo) < 0)
			{
				fprintf(stderr, "error in passing token\n");
				exit (-1);
			}
		}
		else
		{
			if (receiveToken(tokenHandler->sock, tokenHandler->theFileInfo, tokenHandler->neighborInfo) < 0)
			{
				fprintf(stderr, "error in receiving token\n");
				exit (-1);
			}
		}
		i++;
	}
	*/
	/*
	theFileInfo = firstReadWrite(P0, sockfd);
	printf("after first read write\n");
	tokenHandlerInfo = createTokenHandlerStruct(theFileInfo, response);
	tokenHandlerInfo->sock = sockfd;
	pthread_create(&threadBB, NULL, &bbOptions, theFileInfo); // bulletin board thread
	printf("back from thread\n");
	handleTokenWork(tokenHandlerInfo);
	//pthread_create(&threadToken, NULL, &handleTokenWork, tokenHandlerInfo); // token passing thread
	pthread_join(threadBB, (void *)NULL);
	//pthread_join(threadToken, NULL);
	*/

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
	pthread_join(threadBB, (void *)NULL);
	closeSocket(sockfd);
	return 0;
}

