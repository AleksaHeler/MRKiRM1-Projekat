// UDP server that uses blocking sockets

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

#define SERVER_PORT 27015	// Port number of server that will be used for communication with clients
#define BUFFER_SIZE 512		// Size of buffer that will be used for sending and receiving messages to clients


int main()
{
	sockaddr_in serverAddress;								/* Server address */
	char dataBuffer[BUFFER_SIZE];							/* Buffer we will use to send and receive messages */
	WSADATA wsaData;										/* WSADATA data structure that is to receive details of the Windows Sockets implementation */
	int iResult = 0;											/* Keep track of functions return result */

	/* Initialize windows sockets for this process */
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("WSAStartup failed with error: %d\n", WSAGetLastError());
		return 1;
	}


	/* Initialize address structure (used by bind function) */
	memset((char*)&serverAddress, 0, sizeof(serverAddress));	// Initialize memory for address structure
	serverAddress.sin_family = AF_INET; 						// set server address protocol family
	serverAddress.sin_addr = in4addr_any;						// use all available addresses of server
	serverAddress.sin_port = htons(SERVER_PORT);				// Set server port
	

	/* Create a socket */
	SOCKET serverSocket = socket(AF_INET,		/* IPv4 address famly */
		SOCK_DGRAM,								/* Datagram socket */
		IPPROTO_UDP);							/* UDP protocol */

	/* Check if socket creation succeeded */
	if (serverSocket == INVALID_SOCKET)
	{
		printf("Creating socket failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}


	/* Bind server address structure(type, port number and local address) to socket */
	if (bind(serverSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
	{
		printf("Socket bind failed with error: %d\n", WSAGetLastError());
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}


	/* Main loop */
	printf("=== UDP server waiting client messages ===\n");
	while (1)
	{
		// Declare and initialize client address that will be set from recvfrom
		sockaddr_in clientAddress;
		memset(&clientAddress, 0, sizeof(clientAddress));

		// Set whole buffer to zero
		memset(dataBuffer, 0, BUFFER_SIZE);

		// size of client address
		int sockAddrLen = sizeof(clientAddress);

		// Receive client message
		iResult = recvfrom(serverSocket,						// Own socket
			dataBuffer,							// Buffer that will be used for receiving message
			BUFFER_SIZE,							// Maximal size of buffer
			0,									// No flags
			(struct sockaddr*)&clientAddress,	// Client information from received message (ip address and port)
			&sockAddrLen);						// Size of sockadd_in structure

		// Check if message is succesfully received
		if (iResult == SOCKET_ERROR)
		{
			printf("recvfrom failed with error: %d\n", WSAGetLastError());
			continue;
		}

		char ipAddress[INET6_ADDRSTRLEN]; // INET6_ADDRSTRLEN 65 spaces for hexadecimal notation of IPv6

		// Copy client ip to local char[]
		inet_ntop(clientAddress.sin_family, &clientAddress.sin_addr, ipAddress, sizeof(ipAddress));

		// Convert port number from network byte order to host byte order
		unsigned short clientPort = ntohs(clientAddress.sin_port);

		char ipAddress1[15]; // 15 spaces for decimal notation (for example: "192.168.100.200") + '\0'
		struct in_addr* ipv4 = (struct in_addr*)&((char*)&clientAddress.sin_addr)[12];

		// Copy client ip to local char[]
		strcpy_s(ipAddress1, sizeof(ipAddress1), inet_ntoa(clientAddress.sin_addr));
		printf("IPv4 Client connected from ip: %s, port: %d, sent: %s.\n", ipAddress1, clientPort, dataBuffer);


		/// Reply to client /////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////
		char return_msg[64] = "--- !got your message! ---";
		iResult = sendto(serverSocket,
			return_msg,
			strlen(return_msg),
			0,
			(SOCKADDR*)&clientAddress,
			sizeof(clientAddress));
		// Check if message is succesfully sent. If not, close application
		if (iResult == SOCKET_ERROR)
		{
			printf("sendto failed with error: %d\n", WSAGetLastError());
			closesocket(serverSocket);
			WSACleanup();
			return 1;
		}
		/////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////
	}


	/* Close server application */
	iResult = closesocket(serverSocket);
	if (iResult == SOCKET_ERROR)
	{
		printf("closesocket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	/* Close Winsock library */
	WSACleanup();

	return 0;
}
