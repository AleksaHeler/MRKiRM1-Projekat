#include "Const.h"
#include "MailServer.h"
#include "MailClient.h"

// TODO: call Client.SendData("Hello world!");

// FSM system instance
static FSMSystem sys(CLIENT_COUNT + 1 /* max number of automates types */, 2 /* max number of msg boxes */);
Server server;
Client clients[CLIENT_COUNT];


DWORD WINAPI SystemThread(void* data) {

	// Kernel buffer description block
	const uint8 myBuffClassNo = 4;										// number of buffer types
	uint32 myBuffsCount[myBuffClassNo] = { 50, 50, 5000, 10 };			// number of buffers of each buffer type
	uint32 myBuffsLength[myBuffClassNo] = { 256, 256, 512, 1024 };		// buffer size for each buffer type

	// Logging setting 
	LogFile lf("log.log" /*log file name*/, "./log.ini" /* message translator file */);
	LogAutomateNew::SetLogInterface(&lf);

	// Mandatory kernel initialization 
	printf("[*] Initializing system...\n");
	sys.InitKernel(myBuffClassNo, myBuffsCount, myBuffsLength, 2);


	// Add Clients automates to the system 
	sys.Add(&clients[0], CLIENT_TYPE_ID, CLIENT_COUNT, true);
	for (int i = 1; i < CLIENT_COUNT; i++)
	{
		sys.Add(&clients[i], CLIENT_TYPE_ID);
	}

	// Add Server automate to the system 
	sys.Add(&server, SERVER_TYPE_ID, 1, false);

	// Starts the system - dispatches system messages
	printf("[*] Starting system...\n");
	sys.Start();

	return 0;
}

int main(int argc, char** argv) {
	int iResult;
	DWORD thread_id;
	HANDLE thread_handle;
	WSAData wsaData;

	// WSA initialization
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	// Start main thread
	thread_handle = CreateThread(NULL, 0, SystemThread, NULL, 0, &thread_id);
	Sleep(1000);

	/////////////////////////////////////////////////////////////////////////////////////
	/// USER INPUT //////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////
	printf("Press any key to send data...\n");
	_getch();
	clients[0].SendData("Hello world!");
	Sleep(500);

	// Wait for input to end
	_getch();

	// Notify the system to stop - this causes the thread to finish
	printf("[*] Stopping system...\n");
	WSACleanup();
	sys.StopSystem();

	// Free the thread handle
	CloseHandle(thread_handle);

	return 0;
}