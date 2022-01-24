#ifndef __NET_AUTOMAT__
#define __NET_AUTOMAT__

#include <stdio.h>
#include "stdlib.h"
#include "kernel\NetFSM.h"
#include "kernel\errorObject.h"
#include "kernel\stdMsgpc16pl16.h"
#include "const.h"

class net_automat : public NetFSM {
private:

	// NetFSM
	uint16 convertNetToFSMMessage();
	void convertFSMToNetMessage();
	uint8 getProtocolInfoCoding();

	// FSM
	StandardMessage StandardMsgCoding;
	MessageInterface* GetMessageInterface(uint32 id);
	void SetDefaultHeader(uint8 infoCoding);
	uint8 GetMbxId();
	uint8 GetAutomate();
	void SetDefaultFSMData();
	void NoFreeInstances();
	uint8 text[20];
	uint32 msgNumber;
	uint32 idToMsg;

	// Fuunkcije prelaza stanje IDLE
	void net_automat_IDLE_START();
	void net_automat_IDLE_MSG();

	// Stanje MSG
	void net_automat_MSG_MSG();
	void net_automat_MSG_STOP();

	// Neocekivane poruke u stanjima IDLE i MSG
	void net_automat_UNEXPECTED_IDLE();
	void net_automat_UNEXPECTED_MSG();

	// Da ne bude apstraktna klasa
	uint32 getTPKTLen() { return 0; };
	uint32 getLenFromTPKT(unsigned char* buff, int oldMessage = 0) { return 0; };
	void setTPKT(uint16 sendMsgLen, unsigned char* place) {};

public:
	net_automat();
	~net_automat() {};
	void Initialize();
	void StartDemo();
};
#endif