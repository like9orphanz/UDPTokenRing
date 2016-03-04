#ifndef UDPCLIENT_h
#define UDPCLIENT_h

/*
 * Sam Stein
 * Joshua Wright
 * 2/12/16
 *
 * UDPclient.h
 */

#include <netinet/in.h>

typedef struct fileInfo *fileInfoP;

struct fileInfo
{
	int tokenFlag;
	int count;
};

int createSocket();
int sendRequest(int, char * request, char *, int);
int receiveResponse(int, struct sockaddr_in *, int);
void printResponse(struct sockaddr_in *);
int closeSocket(int);
int getAllPeerInfo(int, struct sockaddr_in *, int);
int amIPeerZero(int, struct sockaddr_in *, int);
void createFile();
void removeFile();
int doIHoldTheToken(char *);
void appendFile(fileInfoP);
fileInfoP firstReadWrite(int, int, int);
char * getMessage();
void * bbOptions();
void readFile();
void listFile();
void exitFile();

#endif