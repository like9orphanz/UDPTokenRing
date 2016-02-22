/*
 * Sam Stein
 * Joshua Wright
 *
 * UDPclient.java
 * Systems and Networks II
 * Project 2
 *
 * This file describes the functions to be implemented by the UDPclient class
 * You may also implement any helper functions you deem necessary to complete the project.
 */

import java.io.*;
import java.util.Scanner;
import java.net.*;

public class UDPclient
{
	private DatagramSocket _socket; // the socket for communication with a server
	
	/**
	 * Constructs a TCPclient object.
	 */
	public UDPclient()
	{
	}
	
	/**
	 * Creates a datagram socket and binds it to a free port.
	 *
	 * @return - 0 or a negative number describing an error code if the connection could not be established
	 */
	public int createSocket()
	{	
	
		try {
			_socket = new DatagramSocket();  // Create datagram socket
		} catch(SocketException e) {
			System.out.println("Socket could not be opened");  // Couldn't open socket
			return 0;
		} catch (SecurityException e) {
			System.out.println("Security Exception creating socket");  // Security exception
			return 0;
		} catch (Exception e) {
			e.printStackTrace();  // Any other exception
			return 0;
		}
		
		return 1;
	}

	/**
	 * Sends a request for service to the server. Do not wait for a reply in this function. This will be
	 * an asynchronous call to the server.
	 * 
	 * @param request - the request to be sent
	 * @param hostAddr - the ip or hostname of the server
	 * @param port - the port number of the server
	 *
	 * @return - 0, if no error; otherwise, a negative number indicating the error
	 */
	public int sendRequest(String request, String hostAddr, int port)
	{	
		byte[] message = request.getBytes();  // Convert string to byte array
		
		try {
			DatagramPacket packet = new DatagramPacket(message, message.length, InetAddress.getByName(hostAddr), port);  // Create datagram packet
			_socket.send(packet);  // Send the packet
		} catch (IOException e) {
			System.out.println("IO exception, unable to send request");  // IO exception
			return -1;
		} catch (NullPointerException e){ 
			System.out.println("Null pointer exception sending request");  // Null pointer exception
			return -1;
		}

		return 0;
	}

	/**
	 * Receives the server's response following a previously sent request.
	 *
	 * @return - the server's response or NULL if an error occurred
	 */
	public String receiveResponse()
	{
		byte[] readData = new byte[256];  // buffer
		String serverResponse;  //convert byte[] buffer to string

		try {
			DatagramPacket packet = new DatagramPacket(readData, readData.length);  // create datagram packet
			_socket.receive(packet);  // receive the datagram packet
		} catch (IOException e) {
			System.out.println("IO exception, unable to receive response");  // IO exception
			return "NULL";
		}

		try {
			serverResponse = new String(readData, "UTF-8");  // Convert byte[] to string 
		}
		catch (IOException e) {
			System.out.println("Exception converting byte[] to string");  // Error in conversion
			return "NULL";
		}
		return serverResponse;
	}
	
	/*
    * Prints the response to the screen in a formatted way.
    *
    * response - the server's response as an XML formatted string
    */
	public static void printResponse(String response)
	{
		System.out.println(response);	
	}
 

	/*
	 * Closes an open socket.
	 *
    * @return - 0, if no error; otherwise, a negative number indicating the error
	 */
	public int closeSocket() 
	{
		try{
			_socket.close();  // Close the socket	
		} catch(Exception e) {
			System.out.println("Error closing socket");  // Error closing the socket
			return -1;
		}
		
		return 0;
	}
}
