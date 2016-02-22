/*
 * Sam Stein
 * Joshua Wright
 * 2/12/16
 *
 * UDPclient.h
 */


int createSocket();

int sendRequest(int, char *, char *, int);

int receiveResponse(int, char *, int);

void sendResponse(char *);

int closeSocket(int);
