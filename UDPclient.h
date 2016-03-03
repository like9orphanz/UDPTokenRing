#ifndef UDPCLIENT_h
#define UDPCLIENT_h

/*
 * Sam Stein
 * Joshua Wright
 * 2/12/16
 *
 * UDPclient.h
 */

typedef struct fileInfo *fileInfoP;

struct fileInfo
{
	int tokenFlag;
	int count;
};

int createSocket();

int sendRequest(int, char *, char *, int);

void sendResponse(char *);

int closeSocket(int);

void *bbOptions();

#endif