/*
 *	File Name: TCPclient.c
 *	Author:    Joshua Wright
 *	           Sam Stein
 *	Project: 3
 *
 */


#include "UDPclient.h"
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#define BUFFERSIZE 256

/*
 *	This program is a TCPclient that communicates to TCPserver. This program uses TCPmain 
 *		to send a certain ammount of messages and tests to see if those messages
 *		fit the criteria that the server replies with
 *
 */

/*
 * return value - the socket identifier or a negative number indicating the error 
 * 		  for creating a socket
 * dalfijadsofijasd
 */
int createSocket()
{
	int sockfd;
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		fprintf(stderr, "ERROR opening socket");

	struct sockaddr_in printSock;
	socklen_t addrSize = sizeof(struct sockaddr_in);
	getsockname(sockfd, (struct sockaddr *)&printSock, &addrSize);
	return sockfd;
}

/*
 * Sends a request for service to the server. This is an asynchronous call to the server, 
 * so do not wait for a reply in this function.
 * 
 * sockFD     - the socket identifier
 * request    - the request to be sent encoded as a string
 * serverName - the ip address or hostname of the server given as a string
 * serverPort - the port number of the server
 *
 * return   - 0, if no error; otherwise, a negative number indicating the error
 */
int sendRequest(int sockFD, char * request, char * serverName, int serverPort)
{
	printf("Sending request\n");
	printf("Request: %s\n", request);
	int errorCheck = 0;
	struct hostent * htptr;
	struct sockaddr_in dest;

	if((htptr = gethostbyname(serverName)) == NULL)
	{
		fprintf(stderr, "Invalid host name\n");
		return -1;
	}

	memset(&dest, 0, sizeof(dest));
	dest.sin_family = AF_INET;
	dest.sin_port = htons(serverPort);
	dest.sin_addr = *((struct in_addr *)htptr->h_addr);	

	errorCheck = sendto(sockFD, request, strlen(request), 0, (const struct sockaddr *)&dest, sizeof(dest));
	
	return errorCheck;
}

/*
 * Receives the server's response formatted as an XML text string.
 *
 * sockfd    - the socket identifier
 * response  - the server's response as an XML formatted string to be filled in by this function into the specified string array
 * size      - the size of the buffer for storing a response from the server
 * 
 * return   - 0, if no error; otherwise, a negative number indicating the error
 */
int receiveResponse(int sockFd, struct sockaddr_in *response, int size)
{
	int errorCheck = 0;
	errorCheck = recvfrom(sockFd, (void *)response, size, 0, NULL, NULL);
	return errorCheck;	
}

/*
 * Prints the response to the screen in a formatted way.
 *
 * response - the server's response as an XML formatted string
 *
 */
void printResponse(struct sockaddr_in *response)
{	
	printf("Neighbor ip Address is: %s and port number is %d\n", inet_ntoa(response->sin_addr), htons(response->sin_port));
}

/*
 * Closes the specified socket
 *
 * sockFD - the ID of the socket to be closed
 * 
 * return - 0, if no error; otherwise, a negative number indicating the error
 */
int closeSocket(int sockFD)
{
	int errorCheck = 0;
	errorCheck = close(sockFD);
	return errorCheck;
}

int getAllPeerInfo(int sockFd, struct sockaddr_in *response, int size)
{	
	if (receiveResponse(sockFd, &response[0], size) < 0)
		return -1;
	if (receiveResponse(sockFd, &response[1], size) < 0)
		return -1;
	printf("At 0:\n");
	printResponse(&response[0]);
	printf("At 1:\n");
	printResponse(&response[1]);
	return 0;
}

/*
 * Receives message from server telling this peer if it is peer 0 or not
 *  
 * Returns -1 on error, 0 on not peer 0, and 1 for peer 0.
 */
int amIPeerZero(int sockFD, struct sockaddr_in *P0Response, int size)
{	
	char buffer[256];
	bzero(buffer, 256);
	socklen_t addr_size;
	ssize_t len = recvfrom(sockFD, buffer, sizeof(buffer), 0, (struct sockaddr*)P0Response, &addr_size);
	
	if (len < 0) return -1;
	
	//printf("Response: %s\n", response);
	if (strcmp("no", buffer) == 0)
		return 0;
	else
		return 1;
}

void createFile()
{
	char command[256];
	strcpy(command, "touch filenameBulletinBoard.txt"); 
	system(command);
}

void removeFile(char *fileName)
{
	char command[256];
	strcpy(command, "rm filenameBulletinBoard.txt");
	system(command);
}

int doIHoldTheToken(char *flag)
{
	if (strcmp("no", flag) == 0)
		return 1;
	else return 0;
}

fileInfoP firstReadWrite(int P0, int sockfd)
{	
	fileInfoP thisFileInfo = (fileInfoP) malloc(sizeof(struct fileInfo));

	if (P0 < 0)
	{
		fprintf(stderr,"Error in peer 0 assignment\n");
		closeSocket (sockfd);
		exit (-1);
	}
	else if (P0 == 0) 
	{	
		thisFileInfo->tokenFlag = 0;
		thisFileInfo->count = 0;
	}
	else 
	{	
		printf("I have token\n");
		thisFileInfo->tokenFlag = 1;
		thisFileInfo->count = 1;
		createFile();
	}
	
	return thisFileInfo;
}

tokenHandlerStructP createTokenHandlerStruct(fileInfoP thisFileInfo, struct sockaddr_in *response)
{
		tokenHandlerStructP thisTokenHandlerStruct = (tokenHandlerStructP) malloc(sizeof(struct tokenHandlerStruct));
		thisTokenHandlerStruct->theFileInfo = thisFileInfo;
		thisTokenHandlerStruct->neighborInfo = response;

		return thisTokenHandlerStruct;
}

void readWrite(fileInfoP thisFileInfo)
{
	pthread_t thread;
	pthread_attr_t attr;
	pthread_attr_init (&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	pthread_create(&thread, NULL, &bbOptions, thisFileInfo);
	pthread_join(thread, NULL);
}

int passToken(int sockfd, fileInfoP thisFileInfo, struct sockaddr_in *neighbor)
{	
	printf("passing token to IP: %s, Port No: %d\n", inet_ntoa(neighbor[1].sin_addr), htons(neighbor[1].sin_port));
	int a = sendto(sockfd, &thisFileInfo, sizeof(thisFileInfo), 0, (const struct sockaddr *) &neighbor[1], sizeof(neighbor[1]));
	thisFileInfo->tokenFlag = 0;
	return a;
}

int receiveToken(int sockfd, fileInfoP thisFileInfo, struct sockaddr_in *neighbor)
{
	socklen_t addr_size = sizeof(neighbor[0]);
	ssize_t len = recvfrom(sockfd, thisFileInfo, sizeof(thisFileInfo), 0, (struct sockaddr *) &neighbor[0], &addr_size); 
	
	thisFileInfo->tokenFlag = 1;
	printf("Received token, count now = %d.\n", thisFileInfo->count);
	return len;
}
/*
void *handleTokenWork(void *kablah)
{
	tokenHandlerStructP tokenStruct = (tokenHandlerStructP) kablah;
	printf("in handleTokenWork\n");
	while (1)
	{
		while (tokenStruct->theFileInfo->tokenFlag == 1)
		{
			//allow bb handler to do what it wants while holding token
			if (tokenStruct->theFileInfo->tokenFlag == 0)
			{
				printf("token flag set = 0\n");
				passToken(tokenStruct);
			}
		}
		if (receiveToken(tokenStruct->sock, tokenStruct->theFileInfo, &tokenStruct->neighborInfo[0]) < 0)
		{
			fprintf(stderr, "error in receiving token!\n");
			exit (-1);
		}
	}
}
*/
void *bbOptions(void *blah)
{
	fileInfoP threadFileInfo = (fileInfoP)blah;
	fflush(stdin);

	while (1) {
		int option = 0;
		printf("For write press 1\n");
		printf("For read press 2\n");
		printf("For list press 3\n");
		printf("For exit press 4\n");
		scanf("%d", &option);
		if(option == 1)
			appendFile(threadFileInfo);
		else if(option == 2)
			readFile(threadFileInfo);
		else if(option == 3)
			listFile(threadFileInfo);
		else
			exitFile(threadFileInfo);
	}
}
	
void appendFile(fileInfoP theInfo)
{
	pthread_mutex_t lock;
	FILE *fp;
	
	if(theInfo->count > 1)
	{
		if((fp = fopen("filenameBulletinBoard.txt", "a")) == NULL)
		{
			printf("couldnt open file\n");
			exit(1);
		}
	}
	else
	{
		if((fp = fopen("filenameBulletinBoard.txt", "w")) == NULL)
		{
			printf("couldnt open file\n");
			exit(1);
		}
	}

	char *buffer = getMessage();
	while (1)
	{
		if (theInfo->tokenFlag == 1)
		{
			pthread_mutex_lock(&lock);
			fprintf(fp, "<message n=%d>\n", theInfo->count);
			fprintf(fp, "%s", buffer);
			fprintf(fp, "</message>\n");
			printf("Wrote to file\n");
			theInfo->count++;
			break;
		}
	}

	if (fclose(fp) != 0)
	{
		fprintf(stderr, "error closing file\n");
		exit (-1);
	}
	pthread_mutex_unlock(&lock);
	theInfo->tokenFlag = 0;
}

char *getMessage(fileInfoP thisFileInfo)
{	
	char c;
	char *message = (char *) malloc(256 * sizeof(char));

	c = getchar();
	printf("Enter the message you'd like to post on the bulletin board%c", c);
	fgets(message, 256, stdin);
	message[255] = '\0';
	return message;
}

void readFile(fileInfoP thisFileInfo)
{	
	char temp;
	int messageNumber;
	char *messageNumChar = malloc(sizeof(char) * 500);
	char *message = malloc(sizeof(char) * 500); 
	thisFileInfo->tokenFlag = 0;

	message = "<message n=";
	FILE *fp;
	
	if((fp = fopen("filenameBulletinBoard.txt", "r")) == NULL)
	{
		printf("coulnt open file\n");
		exit(1);
	}
	printf("Enter the message you want to read?\n");
	scanf("%d", &messageNumber);
	sprintf(messageNumChar, "%d", messageNumber);
	char target[strlen(message) + strlen(messageNumChar) + 1];
	strcpy(target, message);
	strcat(target, messageNumChar);
	strcat(target, ">");
	char buffer[1024];
	bzero(buffer, 1024);
 
	while(!feof(fp))
	{
		fgets(buffer, 1024, fp);
		if((strstr(buffer, target)) != NULL)
		{
			bzero(buffer, 1024);
			fgets(buffer, 1024, fp);
			printf("%s\n", buffer);
			while(!feof(fp))
			{	
				temp = getc(fp);
				if (temp == '<')
				{
					temp = getc(fp);
					if(temp == '/')
						break;
				}
				printf("%c", temp);
							
			}		
		}
	}
	if (fclose(fp) != 0)
	{
		fprintf(stderr,"error closing file\n");
		exit(-1);
	}
}
void listFile(fileInfoP thisFileInfo)
{
	printf("list\n");
}
void exitFile(fileInfoP thisFileInfo)
{
	printf("exit\n");
	exit(1);
}
