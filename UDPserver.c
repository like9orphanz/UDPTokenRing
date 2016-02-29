/* 
 * File:   UDPserver.c
 * Author: Joshua Wright
 *	   	   Sam Stein
 *
 * Created on January 13, 2016, 6:08 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <signal.h>
#include <stdint.h>
#include <unistd.h>
/*
 *	This program is to simulate a server that can communicate with a client and 
 *	respond accordingly. Depending on what message the client sends the server will 
 *	send either <reply></reply>, the load average, or <error></error>
 *
 */
int ListenSockCreation(int port, struct sockaddr_in *address);
void printHostInfo();
int processInfo(char *buffer, char *rcvString);
void unknownError(char *buffer, char *rcvString);
int echo(char * buffer);
void changeEcho(char * buffer, char * rcvString);
int pShutdown(char * buffer);
int loadAvg(char * buffer);
void changeLoadAvg(char * buffer, char * rcvString);
int getLoad(char *store);
void portInfo(struct sockaddr_in *serverAddress, int sockfd);
void sendNeighbors(int ls, int numHosts, struct sockaddr_in *clientAddress);
void neighborSpecificInfo(int ls, struct sockaddr_in *clientAddress, int whichNeighbor, int whichHost);
void assignPeerZero(int ls, struct sockaddr_in *clientAddress, int howManyHosts);
/*
 *
 *
 *
 *
 */
int main(int argc, char** argv)
{
	if(argc != 3)
        {
                fprintf(stderr, "Please input <port> <numberHosts>\n");
                exit(1);
        }

	//Listen socket descriptor (reference)
	int ls;
	//Number of bytes to send or reciever
	int len = 0;
	int numberHosts = atoi(argv[2]) ;
	//Server address, and client address
	struct sockaddr_in serverAddress, clientAddress[numberHosts];
	socklen_t addr_size;
	addr_size = sizeof(clientAddress);
	char sentMessage[256];
	int errorCheck = 0;
	int i;

	// Bind the socket, assign numberHosts
	ls = ListenSockCreation(atoi(argv[1]), &serverAddress);

	printHostInfo();
	portInfo(&serverAddress, ls);

	char buffer[256];
	
	for (i = 0 ; i < numberHosts ; i++) 
	{
		bzero(buffer, 256);
		len = recvfrom(ls, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientAddress[i], &addr_size);
		fprintf(stderr, "Connected with %s at port %d\n", inet_ntoa(clientAddress[i].sin_addr), htons(clientAddress[i].sin_port));
		if(len < 0)
			fprintf(stderr, "ERROR in recvfrom\n");
	}

	for (i = 0; i < numberHosts; i++) 
	{
		printf("Client address %d is %s\n", i, inet_ntoa(clientAddress[i].sin_addr));
		printf("Port number %d is %d\n", i, htons(clientAddress[i].sin_port));
	}

	sendNeighbors(ls, numberHosts, clientAddress);
	printf("made it\n");
	assignPeerZero(ls, clientAddress, numberHosts);
	
	close(ls);
	return(EXIT_SUCCESS);
}
/*
 * Prints out information for the Server
 *
 * 	Hostname
 * 	IP Address
 *
 */
void sendNeighbors(int ls, int numHosts, struct sockaddr_in *clientAddress)
{
	int i;
	int errorCheck = 0;

	for(i = 0; i < numHosts; i++)
	{
		if (i == 0 && numHosts != 2) 
		{
			neighborSpecificInfo(ls, clientAddress, numHosts - 1, i);  // left host
			neighborSpecificInfo(ls, clientAddress, 1, i);  // right host
		}
		if (i == 0 && numHosts == 2) 
		{
			neighborSpecificInfo(ls, clientAddress, 1, i); // left host
		}
		if (i == 1 && numHosts == 2)
		{
			neighborSpecificInfo(ls, clientAddress, 0, i); // right host
		}
		if (i == numHosts - 1 && numHosts != 2) 
		{
			neighborSpecificInfo(ls, clientAddress, numHosts - 2, i);  // left host
			neighborSpecificInfo(ls, clientAddress, 0, i);  // right host
		}
		else 
		{
			neighborSpecificInfo(ls, clientAddress, i - 1, i);  // left host
			neighborSpecificInfo(ls, clientAddress, i + 1, i);  // right host
		}
	}
}

void neighborSpecificInfo(int ls, struct sockaddr_in *clientAddress, int whichNeighbor, int whichHost)
{	
	sendto(ls, (void *)&(clientAddress[whichNeighbor]), sizeof(clientAddress[whichNeighbor]), 0, (const struct sockaddr *)&clientAddress[whichHost], sizeof(clientAddress[whichHost]));		
}

void assignPeerZero(int ls, struct sockaddr_in *clientAddress, int howManyHosts) 
{
	char buffer[256];
	bzero(buffer, 256);
	int i;
	for (i = 0 ; i < howManyHosts ; i++) 
	{
		if (i == 0)
		{
			strcpy(buffer, "yes");
			buffer[3] = '\0';
		}
		else 
		{
			strcpy(buffer, "no");
			buffer[2] = '\0';
		}
		sendto(ls, buffer, sizeof(buffer), 0, (const struct sockaddr *)&clientAddress[i], sizeof(clientAddress[i]));
	}
}

void printHostInfo()
{
        char hostname[1024];
        hostname[1023] = '\0';
        struct hostent * hostptr;
        gethostname(hostname, 1023);
        //find the ip address
        hostptr = gethostbyname(hostname);
        fprintf(stderr, "Host Name: %s\n", hostname);
        fprintf(stderr, "IP address: %s\n", inet_ntoa(*(struct in_addr*)hostptr->h_addr));   }
/*
 * Prints out the port number
 *
 * sockfd		-The socket identifier
 * serverAddress	-The server's address information; the structure should be created 
 * 			 with information on the server in this function call
 *
 *
 */        
void portInfo(struct sockaddr_in *serverAddress, int sockfd)
{
        struct sockaddr_in printSock;
        socklen_t addrLen = sizeof(struct sockaddr);
        getsockname(sockfd, (struct sockaddr *)&printSock, &addrLen);
        fprintf(stderr, "Sock port: %d\n", ntohs(printSock.sin_port));
}
/*
 * reads the message that the client sends and depending on what the message says 
 * responds accordingly
 *
 * buffer	-The array that holds the message sent from the client
 * rcvString	-The array holding the changes made to the message depending
 * 		 on what the message was that the client sent
 *
 */
int processInfo(char *buffer, char *rcvString)
{
    // Initialize
	int errorCheck = 0, response = 0;  
	int length = strlen(buffer);
	if(length > 256)
		return response;
	bzero(rcvString, 256);
	
	// Check for different commands
	if((errorCheck = pShutdown(buffer)) == 0)
		response = 1;
	else if((errorCheck = echo(buffer)) != 0)
		changeEcho(buffer, rcvString);
	else if((errorCheck = loadAvg(buffer)) != 0)
		changeLoadAvg(buffer, rcvString);
	else
		unknownError(buffer, rcvString);
	
	return response;
}
/*
 *  Handles unknown command format
 *
 *  buffer	-Array holding the original message sent from the client
 *  rcvString	-Array to hold the changes made to the original message depending
 *  		 on what the message was that the client sent
 */
void unknownError(char *buffer, char *rcvString)
{
	bzero(rcvString, 256);
	strcpy(rcvString, "<error>unknown format</error>");
}
/*
 * Checks to see if the message is an <echo></echo> message from the client.
 *
 * buffer	-Array holding the original message
 */
int echo(char * buffer)
{
	//response is the int that is going to be sent back to the calling function
	int response = 0;
	//array to put <echo>
	char dest[256];
	//filling the dest array with null values
	bzero(dest, 256);
	//puts the first 6 chars in the dest array
	strxfrm(dest, buffer, 6);
	//needle in the haystack search: Searches for </echo> 
	//inside the original message and puts it in the array end.
	char *end = strstr(buffer, "</echo>");
	//if end is not null then it goes inside the if statement
        if(end != NULL)
	{
		//if dest contains <echo> and end contains </echo> then response = 1
		if((strcmp(dest, "<echo>") == 0) && (strcmp(end, "</echo>") == 0))
			response = 1;
	}
	//returns response with either 0 or 1 depending on the if statement above
	return response;
}
/*
 * Replaces <echo></echo> with <reply></reply>
 *
 * buffer	-Array containing the original message the client sent
 * rcvString	-Replacement array used to put the changes and the original message in
 *
 */
void changeEcho(char * buffer, char * rcvString)
{
	//setting i and length to 0
	int i = 0, length = 0;
	//array 
	char reply[256];
	char *tp;
	//creates and array to store the message from buffer
	char store[256];
	bzero(rcvString, 256);
	bzero(store, 256);
	bzero(reply, 256);
	//copies the message in buffer to rcvString
	strcpy(rcvString, buffer);
	//searches for > in rcvString and puts in in tp
	tp = strstr(rcvString, ">");
	//sets the size of tp + the size of a char to tp
	tp = tp + sizeof(char);
	//copies the string in tp to rcvString
	memmove(rcvString, tp, 256);
	//sets the length of rcvString to length
	length = strlen(rcvString);
	//creats a temp null variable
	char temp = '\0';
	//while i is less then length
	while(i < length)
	{
		//temp = the first char in rcvString
		temp = rcvString[i];
		//if temp is equal to < 
		if(temp == '<')
			//then i = length
			i = length;
		else
			//if temp doesn't equal < then the store array[i] = the temp variable
			store[i] = temp;
		//add 1 to i
		i++;
	}
	//copies what is the store array to rcvString
	strcpy(rcvString, store);
	//copies <reply> to the reply array
	strcpy(reply, "<reply>");
	//appends the client message to the end of the reply array containing <reply>
	strcat(reply, rcvString);
	//appends to the end of the reply array </reply>
	//which contains <reply>
	//and the client message
	strcat(reply, "</reply>");
	//sets whats is in rcvString to null
	bzero(rcvString, 256);
	//copies the entirety of the reply array to the rcvString array
	strcpy(rcvString, reply);
}
/*
 *  Gracefully shut down the server after being issued the <shutdown/> command
 *
 *  buffer	-Array containing the message the client sent
 */
int pShutdown(char * buffer)
{
	//vairable to be sent back to the calling function
	int response = 1;
	//compares the message sent from the client with <shutdown/>
	if((strcmp(buffer, "<shutdown/>")) == 0)
		//iff the message from the client is <shutdown/> then response = 0
		response = 0;
	//return the value of response
	return response;

}

/*
 *  Compares the message sent from the client and <loadavg/>
 *
 *  buffer	-Array containing the message sent from the client
 */
int loadAvg(char * buffer)
{
	//compares the message sent from the client and <loadavg/>
	if((strcmp(buffer, "<loadavg/>")) == 0)
		//if they are the same then the function returns a 1
		return 1;
	else
		//if not the same the function returns a 0
		return 0;
}
/*
 * Puts <replyLoadAvg> at the begining of the load and
 * puts </replyLoadAvg> at the end of the load
 *
 * buffer	-Array containing the message from the client
 * rcvString	-Array to help alter the message sent from the client
 *
 */
void changeLoadAvg(char * buffer, char * rcvString)
{
	//array to store the load
	char store[256];
	//sets every element of the array to null values
	bzero(store, 256);
	//copies <replyLoadAvg> to rcvSting
	strcpy(rcvString, "<replyLoadAvg>");
	//calls the getLoad function to get the load
	int errorCheck = getLoad(store);
	//if the load equals to -1 then error
	if(errorCheck == -1)
	{
		fprintf(stderr, "Failed getting load aver\n");
		exit(-1);
	}
	//appends the load to the end of rcvString containing <replayLoadAvg>
	strcat(rcvString, store);
	//appends </replyLoadAvg> to the end of rcvString which contains
	//<replyLoadAvg> and the load
	strcat(rcvString, "</replyLoadAvg>");
}

/*
 * Compute the load
 *
 * store	-Array to store the load after recieving it
 */
int getLoad(char *store)
{
	//array to hold 3 loadaverages
	double loadAvg[3];
	//setting the contents of store to null values
	bzero(store, 256);
	int errorCheck = 0;
	//getting 3 load averages and putting them in loadAvg[0-2]
	errorCheck = getloadavg(loadAvg, 3);
	//if getloadavg doesnt come back with an error
	if(errorCheck != -1)
	{
		//stores the laod averages in the store array
		sprintf(store, "%f:%f:%f", loadAvg[0], loadAvg[1], loadAvg[2]);
	}
	//returns the value of errorCheck
	return errorCheck;
}
/*
 * Creates the server's address information 
 *
 * port         -the port number of the server
 * address	-the server's address information; the structure should be created with 
 * 		 information on the server (like port, address, and family) in this function
 * 		 call
 *
 */
int ListenSockCreation(int port, struct sockaddr_in *address)
{
	int sock_ls;

        memset(address, 0, sizeof(*address));
        address->sin_family = AF_INET;
        address->sin_addr.s_addr = htonl(INADDR_ANY);
        address->sin_port = htons(port);

	//creates a socket
	if((sock_ls = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
        {
                fprintf(stderr, "Error: listen sock failed!");
                exit(1);
        }

	//binds the socket
        if(bind(sock_ls, (struct sockaddr *)address, sizeof(*address)) < 0)
        {
                fprintf(stderr, "Error binding\n");
                close(sock_ls);
                exit(1);
        }

	return sock_ls;
}					
