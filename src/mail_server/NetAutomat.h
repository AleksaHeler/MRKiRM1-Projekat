#ifndef __NET_AUTOMAT__
#define __NET_AUTOMAT__

#include <stdio.h>
#include "stdlib.h"
#include "kernel\NetFSM.h"
#include "kernel\errorObject.h"
#include "kernel\stdMsgpc16pl16.h"
#include "const.h"

class NetAutomat : public NetFSM {
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
	void NetAutomat_IDLE_START();
	void NetAutomat_IDLE_MSG();

	// Stanje MSG
	void NetAutomat_MSG_MSG();
	void NetAutomat_MSG_STOP();

	// Neocekivane poruke u stanjima IDLE i MSG
	void NetAutomat_UNEXPECTED_IDLE();
	void NetAutomat_UNEXPECTED_MSG();

	// Da ne bude abstraktna klasa
	uint32 getTPKTLen() { return 0; };
	uint32 getLenFromTPKT(unsigned char* buff, int oldMessage = 0) { return 0; };
	void setTPKT(uint16 sendMsgLen, unsigned char* place) {};

public:
	NetAutomat();
	~NetAutomat() {};
	void Initialize();
	void StartDemo();
};
#endif