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
typedef struct tokenHandlerStruct *tokenHandlerStructP;

struct fileInfo
{
	int tokenFlag;
	int count;
};

struct tokenHandlerStruct
{
	fileInfoP theFileInfo;
	struct sockaddr_in *neighborInfo;
	int sock;
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
fileInfoP firstReadWrite(int, int);
void readWrite(fileInfoP);
int passToken(int, struct sockaddr_in *, fileInfoP);
int receiveToken(int, fileInfoP, struct sockaddr_in *);
void *handleTokenWork(void *);
tokenHandlerStructP createTokenHandlerStruct(fileInfoP, struct sockaddr_in *);
char * getMessage();
void * bbOptions();
void readFile(fileInfoP);
void listFile(fileInfoP);
void exitFile();

#endif