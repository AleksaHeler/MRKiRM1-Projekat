#include "stdio.h"
#include "conio.h"
#include "Kernel/fsmsystem.h"
#include "Kernel/LogFile.h"
#include "NetAutomat.h"

// Ovom definicijom modifikujemo program ako treba da se naprave
// dva programa koji ce biti pokrenuti na istom racunaru
#define AUTOMAT1

// Sistem automata sastoji se od jednog tipa automata koji
// razmenjuje poruke preko jednog postanskog sanduceta
FSMSystemWithTCP sistemAutomata(1, 1);

// Pravimo instancu klase NetAutomat
NetAutomat automat_1;

DWORD WINAPI ThreadFunction(void* dummy) {
	uint32 buffersCount[3] = { 5, 3, 2 };
	uint32 buffersLength[3] = { 128, 256, 512 };
	uint8 buffClassNo = 3;

	// Inicijalizacija sistema automata
	printf("Inicijalizujem FSMSystemWithTCP \n");
	sistemAutomata.Add(&automat_1, FSM_TYPE_AUTOMAT, 1, true);
	sistemAutomata.InitKernel(buffClassNo, buffersCount, buffersLength, 1);
	LogFile lf("log.log", "log.ini");
	LogAutomateNew::SetLogInterface(&lf);

	// Kako je svejedno ko ce pokrenuti slanje poruka u oba automata
	// je uveden server stim sto je za program 1 server na portu PORT_1
	// a za drugi program na portu PORT_2. Sve je jedno koji ce automat prvi
	// pokrenuti TCP komunikaciju sa porukom establishConection().
#ifdef AUTOMAT1
	printf("Startujemo server...na portu:%u\n", PORT_1);
	sistemAutomata.InitTCPServer(PORT_1, FSM_TYPE_AUTOMAT);
#else
	printf("Startujemo server...na portu:%u\n", PORT_2);
	sistemAutomata.InitTCPServer(PORT_2, FSM_TYPE_AUTOMAT);
#endif
	// Pokrecemo sistem automata
	printf("Startujem FSMSystem\n");
	try {
		sistemAutomata.Start();
	}
	catch (...) {
		printf("Exception - prekid rada sistema automata\n");
		return 0;
	}
	printf("Kraj rada sistema automata\n");
	return 0;
}

void main(int argc, char* argv[]) {
	DWORD threadID;
	bool end = false;
	char ret;

	// Pokrecemo obradu poruka sistema automata u posebnoj programskoj niti
	HANDLE hTemp = CreateThread(NULL, 0, ThreadFunction, NULL, 0, &threadID);
	Sleep(100);

	// Program radi dok se sa tastature ne unese karakter Q ili q
	while (!end) {
		if (_kbhit()) {
			ret = _getch();
			switch (ret) {
			case 'Q':
			case 'q':
				sistemAutomata.StopSystem();
				end = true;
				Sleep(100);
				break;
			case 'S':
			case 's':
				automat_1.StartDemo();
				break;
			case 'E':
			case 'e':
				// Pritiskom na e u bilo kom automatu pokrece se uspostavljanje veze
				// sa serverom drugog automata kako bi se uspostavlila veza za razmenu podataka
#ifdef AUTOMAT1
				automat_1.setPort(PORT_2);
				automat_1.setIP(IP_ADDRES);
				printf("establishConection...na portu:%u", PORT_2);
				automat_1.establishConnection();
#else
				automat_1.setPort(PORT_1);
				automat_1.setIP(IP_ADDRES);
				printf("establishConection...na portu:%u", PORT_1);
				automat_1.establishConnection();
#endif
			default:
				break;
			}
		}
	}
	CloseHandle(hTemp);
	printf("Kraj rada \n");
}