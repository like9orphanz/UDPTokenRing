/*
 *	File Name: TCPclient.c
 *	Author:    Joshua Wright
 *	           Sam Stein
 *	Project: 1
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

/*
 *	This program is a TCPclient that communicates to TCPserver. This program uses TCPmain 
 *		to send a certain ammount of messages and tests to see if those messages
 *		fit the criteria that the server replies with
 *
 */

/*
 * return value - the socket identifier or a negative number indicating the error 
 * 		  for creating a socket
 *
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
int receiveResponse(int sockFD, char * response, int size)
{
	int errorCheck = 0;
	errorCheck = recvfrom(sockFD, response, size, 0, NULL, NULL);
	return errorCheck;	
}

/*
 * Prints the response to the screen in a formatted way.
 *
 * response - the server's response as an XML formatted string
 *
 */
void printResponse(char * response)
{	
	int i = 0;
	int a = 0;
	int flag = 0;
	char ip[256];
	char port[256];
	while (response[i] != '\0') {
		if (response[i] == ' ') {
			flag = 1;
			ip[i] = '\0';
		}
		
		if (flag == 0) {
			ip[i] = response[i];
		}
		else {
			port[a] = response[i];
			a++;
		}
		i++;
	}
	printf("Response is: %s %d\n", ip, atoi(port));
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

