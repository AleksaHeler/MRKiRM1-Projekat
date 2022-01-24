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


bool is_ipV4_address(sockaddr_in6 address);

int main()
{
	// Server address structure
	sockaddr_in serverAddress;

	// Size of server address structure
	int sockAddrLen = sizeof(serverAddress);

	// Buffer that will be used for sending and receiving messages to client
	char dataBuffer[BUFFER_SIZE];

	// WSADATA data structure that is used to receive details of the Windows Sockets implementation
	WSADATA wsaData;

	// Initialize windows sockets for this process
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	// Check if library is succesfully initialized
	if (iResult != 0)
	{
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	// Initialize address structure of server
	memset((char*)&serverAddress, 0, sizeof(serverAddress));		// Initialize memory for address structure
	serverAddress.sin_family = AF_INET;								// IPv4 address famly
	serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP_ADDRESS);	// Set server IP address using string
	serverAddress.sin_port = htons(SERVER_PORT);					// Set server port

	// Create a socket
	SOCKET clientSocket = socket(AF_INET,      // IPv4 address famly
		SOCK_DGRAM,   // Datagram socket
		IPPROTO_UDP); // UDP protocol

	// Check if socket creation succeeded
	if (clientSocket == INVALID_SOCKET)
	{
		printf("Creating socket failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	while (1)
	{
		printf("Enter message to send:\n");

		// Read string from user into outgoing buffer
		gets_s(dataBuffer, BUFFER_SIZE);

		// Send message to server
		iResult = sendto(clientSocket,						// Own socket
			dataBuffer,						// Text of message
			strlen(dataBuffer),				// Message size
			0,									// No flags
			(SOCKADDR*)&serverAddress,		// Address structure of server (type, IP address and port)
			sizeof(serverAddress));			// Size of sockadr_in structure

		// Check if message is succesfully sent. If not, close client application
		if (iResult == SOCKET_ERROR)
		{
			printf("sendto failed with error: %d\n", WSAGetLastError());
			closesocket(clientSocket);
			WSACleanup();
			return 1;
		}

		/////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////
		while (1) {
			// Declare and initialize client address that will be set from recvfrom
			sockaddr_in6 clientAddress;
			memset(&clientAddress, 0, sizeof(clientAddress));

			// Set whole buffer to zero
			memset(dataBuffer, 0, BUFFER_SIZE);
			// size of client address
			int sockAddrLen = sizeof(clientAddress);

			// Receive client message
			iResult = recvfrom(clientSocket,						// Own socket
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
			inet_ntop(clientAddress.sin6_family, &clientAddress.sin6_addr, ipAddress, sizeof(ipAddress));

			// Convert port number from network byte order to host byte order
			unsigned short clientPort = ntohs(clientAddress.sin6_port);

			bool isIPv4 = is_ipV4_address(clientAddress); //true for IPv4 and false for IPv6

			if (isIPv4) {
				char ipAddress1[15]; // 15 spaces for decimal notation (for example: "192.168.100.200") + '\0'
				struct in_addr* ipv4 = (struct in_addr*)&((char*)&clientAddress.sin6_addr.u)[12];

				// Copy client ip to local char[]
				strcpy_s(ipAddress1, sizeof(ipAddress1), inet_ntoa(*ipv4));
				printf("IPv4 Client connected from ip: %s, port: %d, sent: %s.\n", ipAddress1, clientPort, dataBuffer);
			}
			else
				printf("IPv6 Client connected from ip: %s, port: %d, sent: %s.\n", ipAddress, clientPort, dataBuffer);
		}
		/////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////
	}
	// Only for demonstration purpose
	printf("Press any key to exit: ");
	_getch();

	// Close client application
	iResult = closesocket(clientSocket);
	if (iResult == SOCKET_ERROR)
	{
		printf("closesocket failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	// Close Winsock library
	WSACleanup();

	// Client has succesfully sent a message
	return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
bool is_ipV4_address(sockaddr_in6 address)
{
	char* check = (char*)&address.sin6_addr.u;

	for (int i = 0; i < 10; i++)
		if (check[i] != 0)
			return false;

	if (check[10] != -1 || check[11] != -1)
		return false;

	return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
