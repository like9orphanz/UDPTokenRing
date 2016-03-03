CFLAGS = -g -Wall
CC = gcc

all : UDPserver UDPclientC

UDPserver : UDPserver.c
	gcc $(CFLAGS) -o UDPserver UDPserver.c

UDPclientC : UDPmain.c UDPclient.c
	gcc $(CFLAGS) -pthread -o UDPclientC UDPmain.c UDPclient.c

clean :
	rm UDPserver UDPclientC
