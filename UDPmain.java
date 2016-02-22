/*
 * 
 * UDPmain.java
 * Systems and Networks II
 * Project 2
 *
 * This file implements the Java test program for testing the UDP client-server communication.
 */

import java.io.*;
import java.net.*;

public class UDPmain {

    /**
     * The main function. Use this function for testing your code. We will use
     * our own main function for testing.
     */
    public static void main(String[] args)
    {
        UDPclient client;
        String    serverName;
        String    req;

        if (args.length != 2) {
            System.err.println("Usage: UDPclient <serverName> <port number>\n");
            return;
        }
        try {
            serverName = args[0];
        } catch (NullPointerException xcp) {
            System.err.println("Usage: UDPclient <serverName> <port number>\n");
            return;
        }

        int portNum;
        try {
            portNum = Integer.parseInt(args[1]);
        } catch (NumberFormatException xcp) {
            System.err.println("Usage: UDPclient <serverName> <port number>\n");
            return;
        }

        // construct client and client socket
        client = new UDPclient();
        if (client.createSocket() < 0) {
            return;
        }
        
        
        System.out.print ("Enter a request: ");
        req = System.console().readLine();
      
        if (client.sendRequest(req, serverName, portNum) < 0) {
			client.closeSocket();
            return;
        }

        String response = client.receiveResponse();
        if (response != null) {
            UDPclient.printResponse(response.trim());
        }
		else {
            System.err.println ("incorrect response from server");
        }

        client.closeSocket();
    
    }
}

