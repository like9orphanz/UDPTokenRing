CFLAGS = -g -Wall
CC = gcc
JC = javac
JFLAGS = -g


all : UDPserver UDPclientC UDPclient

UDPserver : UDPserver.c
	gcc -o UDPserver UDPserver.c

UDPclientC : UDPmain.c UDPclient.c
	gcc -o UDPclientC UDPmain.c UDPclient.c

UDPclient : UDPmain.java UDPclient.java
	javac UDPmain.java UDPclient.java
clean :
	rm UDPserver UDPclientC UDPclient.class UDPmain.class
