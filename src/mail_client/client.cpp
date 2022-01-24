// UDP client that uses blocking sockets

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "conio.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define SERVER_IP_ADDRESS "127.0.0.1"		// IPv4 address of server
#define SERVER_PORT 27015					// Port number of server that will be used for communication with clients
#define BUFFER_SIZE 512						// Size of buffer that will be used for sending and receiving messages to client


int main()
{
	sockaddr_in serverAddress;									/* Server address structure */
	char dataBuffer[BUFFER_SIZE];								/* Buffer that will be used for sending and receiving messages */
	WSADATA wsaData;											/* WSADATA data structure that is used to receive details of the Windows Sockets implementation */
	int iResult = 0;											/* Keep track of functions return result */

	/* Initialize windows sockets for this process */
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("WSAStartup failed with error: %d\n", WSAGetLastError());
		return 1;
	}


	/* Initialize address structure of server */
	memset((char*)&serverAddress, 0, sizeof(serverAddress));		/* Initialize memory for address structure */
	serverAddress.sin_family = AF_INET;								/* IPv4 address famly */
	serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP_ADDRESS);	/* Set server IP address using string */
	serverAddress.sin_port = htons(SERVER_PORT);					/* Set server port */


	/* Create a socket */
	SOCKET clientSocket = socket(AF_INET,		/* IPv4 address famly */
		SOCK_DGRAM,								/* Datagram socket */
		IPPROTO_UDP);							/* UDP protocol */

	/* Check if socket creation succeeded */
	if (clientSocket == INVALID_SOCKET)
	{
		printf("Creating socket failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}


	/* Main loop */
	printf("=== UDP client ready to send messages ===\n");
	while (1)
	{
		printf("Enter message to send:\n");
		gets_s(dataBuffer, BUFFER_SIZE);

		// Send message to server
		iResult = sendto(clientSocket,			// Own socket
			dataBuffer,							// Text of message
			strlen(dataBuffer),					// Message size
			0,									// No flags
			(SOCKADDR*)&serverAddress,			// Address structure of server (type, IP address and port)
			sizeof(serverAddress));				// Size of sockadr_in structure

		// Check if message is succesfully sent. If not, close client application
		if (iResult == SOCKET_ERROR)
		{
			printf("sendto failed with error: %d\n", WSAGetLastError());
			closesocket(clientSocket);
			WSACleanup();
			return 1;
		}

		/// RECEIVE MSG FROM SERVER /////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////
		// Declare and initialize client address that will be set from recvfrom
		sockaddr_in serverAddress;
		memset(&serverAddress, 0, sizeof(serverAddress));
		memset(dataBuffer, 0, BUFFER_SIZE);				// Set whole buffer to zero
		int sockAddrLen = sizeof(serverAddress);		// size of client address

		// Receive client message
		iResult = recvfrom(clientSocket,						// Own socket
			dataBuffer,							// Buffer that will be used for receiving message
			BUFFER_SIZE,							// Maximal size of buffer
			0,									// No flags
			(struct sockaddr*)&serverAddress,	// Client information from received message (ip address and port)
			&sockAddrLen);						// Size of sockadd_in structure

		// Check if message is succesfully received
		if (iResult == SOCKET_ERROR)
		{
			printf("recvfrom failed with error: %d\n", WSAGetLastError());
			continue;
		}

		char ipAddress[INET6_ADDRSTRLEN]; // INET6_ADDRSTRLEN 65 spaces for hexadecimal notation of IPv6

		// Copy client ip to local char[]
		inet_ntop(serverAddress.sin_family, &serverAddress.sin_addr, ipAddress, sizeof(ipAddress));

		// Convert port number from network byte order to host byte order
		unsigned short clientPort = ntohs(serverAddress.sin_port);

		char ipAddress1[64]; // 15 spaces for decimal notation (for example: "192.168.100.200") + '\0'
		struct in_addr* ipv4 = (struct in_addr*)&((char*)&serverAddress.sin_addr)[12];

		// Copy client ip to local char[]
		strcpy_s(ipAddress1, sizeof(ipAddress1), inet_ntoa(*ipv4));
		printf("IPv4 Client connected from ip: %s, port: %d, sent: %s.\n", ipAddress1, clientPort, dataBuffer);
		/////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////
	}
	

	/* Close client application */
	iResult = closesocket(clientSocket);
	if (iResult == SOCKET_ERROR)
	{
		printf("closesocket failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	/* Close Winsock library */
	WSACleanup();

	return 0;
}
