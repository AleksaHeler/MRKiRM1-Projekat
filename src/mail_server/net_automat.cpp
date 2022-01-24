#include "kernel/LogFile.h"
#include "net_automat.h"

net_automat::net_automat() : NetFSM(
	0, //uint8 automateId1
	0, //uint8 mbxId1
	0, //uint16 numOfTimers = DEFAULT_TIMER_NO,
	2, //uint16 numOfState = DEFAULT_STATE_NO,
	3)//uint16 maxNumOfProceduresPerState = DEFAULT_PROCEDURE_NO_PER_STATE
{
	SetDefaultFSMData();
}

// Ova metoda sadrzi informacije o tome koje poruke automat moze da obradi
// Za ovakvu definiciju u okviru klase potrebno je definisati dodatnu
// promenljivu: StandardMessage standardMsgCoding;
MessageInterface* net_automat::GetMessageInterface(uint32 id) {
	switch (id) {
	case 0x00:
		return &StandardMsgCoding;
	}
	throw TErrorObject(__LINE__, __FILE__, 0x01010400);
}

// Ova metoda popunjava zaglavlje poruke koju automati medjusobno razmenjuju
void net_automat::SetDefaultHeader(uint8 infoCoding) {
	SetMsgInfoCoding(infoCoding);
	SetMessageFromData();
}

// Metoda kojom se definise broj postanskog sanduceta iz kog ce automati ove
// klase primati poruke koje su pristigle u sistem automata.
uint8 net_automat::GetMbxId() {
	return MBX_AUTOMAT_ID;
}

// Metoda vraca broj koji identifikuje tip automata kojem pripada ovaj automat
uint8 net_automat::GetAutomate() {
	return FSM_TYPE_AUTOMAT;
}

// Metoda u kojoj se izvrsava inicijalizacija atributa instanci ove klase.
void net_automat::SetDefaultFSMData() {
	msgNumber = 0;
	idToMsg = INVALID_32;
}

// Metoda koja ce se pozvati ako nema vi�e slobodnih automata da obrade
// pristiglu poruku. Ova metoda se koristi ako su automati ove klase predhodno
// dodati u sistem automata sa parametrom useFreeList = true
void net_automat::NoFreeInstances() {}

// Metoda koja inicijalizuje funkcije prelaza i definise vremenske kontrole
// koje ce automati ove klse koristiti u svom radu. Ova metoda se poziva
// implicitno u okviru procesa dodavanja instanci automata u sistem automata.
// Sve funkcije prelaza se posebno deklarisu i definisu.
void net_automat::Initialize() {
	// Ovde ide serija inicijalizacija:
	// InitEventProc(uint8 state, uint16 event, PROC_FUN_PTR fun);
	// InitUnexpectedEventProc(uint8 state, PROC_FUN_PTR fun);
	// InitTimerBlock(uint16 timerId, uint32 timerCount, uint16 signalId);
	InitEventProc(IDLE, IDLE_START, (PROC_FUN_PTR)
		&net_automat::net_automat_IDLE_START);
	InitEventProc(IDLE, IDLE_MSG, (PROC_FUN_PTR)
		&net_automat::net_automat_IDLE_MSG);
	InitEventProc(MESSAGE, MSG_MSG, (PROC_FUN_PTR)
		&net_automat::net_automat_MSG_MSG);
	InitEventProc(MESSAGE, MSG_STOP, (PROC_FUN_PTR)
		&net_automat::net_automat_MSG_STOP);
	InitUnexpectedEventProc(IDLE, (PROC_FUN_PTR)
		&net_automat::net_automat_UNEXPECTED_IDLE);
	InitUnexpectedEventProc(MESSAGE, (PROC_FUN_PTR)
		&net_automat::net_automat_UNEXPECTED_MSG);
}

// Funkcije prelaza za stanje IDLE
void net_automat::net_automat_IDLE_START() {
	msgNumber = 1;
	idToMsg = 0;

	// Postavljamo automat u stanje MSG i
	// saljemo poruku sledecem automatu
	PrepareNewMessage(0x00, IDLE_MSG);
	char text[] = "OVO JE PRVA PORUKA";
	AddParam(PARAM_TEXT, strlen(text), (unsigned char*)text);
	AddParamDWord(COUNT, msgNumber);
	SetMsgToAutomate(FSM_TYPE_AUTOMAT);
	SetMsgToGroup(INVALID_08);
	SetMsgObjectNumberTo(idToMsg);
	sendToTCP();
	SetState(MESSAGE);
}

void net_automat::net_automat_IDLE_MSG() {
	idToMsg = 0;

	// Preuzimamo parametre iz poruke
	unsigned char* tmp;
	tmp = GetParam(PARAM_TEXT);
	assert(tmp);
	memcpy(text, tmp + 2, *(tmp + 1));
	memset(text + (*(tmp + 1)), 0x00, 1); // pravimo string
	GetParamDWord(COUNT, msgNumber);
	printf("Primljeni tekst: %s\n", text);

	// Ako je broj poruka manji od zadatog
	// nastavi sa brojanjem u suprotnom kraj
	if (msgNumber < MAX_MSG_NUM) {
		msgNumber++;

		// Postavljamo automat u stanje MSG i
		// saljemo poruku sledecem automatu
		PrepareNewMessage(0x00, MSG_MSG);
		AddParamDWord(COUNT, msgNumber);
		SetMsgToAutomate(FSM_TYPE_AUTOMAT);
		SetMsgToGroup(INVALID_08);
		SetMsgObjectNumberTo(idToMsg);
		sendToTCP();
		SetState(MESSAGE);
	}
	else {
		printf("Stop automat: %u, poruka %u\n", GetObjectId(), msgNumber);

		// Postavljamo automat u stanje IDLE i
		// saljemo poruku sledecem automatu
		PrepareNewMessage(0x00, MSG_STOP);
		SetMsgToAutomate(FSM_TYPE_AUTOMAT);
		SetMsgToGroup(INVALID_08);
		SetMsgObjectNumberTo(idToMsg);
		sendToTCP();
		SetState(IDLE);
	}
}

void net_automat::net_automat_MSG_MSG() {
	GetParamDWord(COUNT, msgNumber);
	msgNumber++;

	if (msgNumber < MAX_MSG_NUM) {
		// saljemo poruku sledecem automatu
		PrepareNewMessage(0x00, MSG_MSG);
		AddParamDWord(COUNT, msgNumber);
		SetMsgToAutomate(FSM_TYPE_AUTOMAT);
		SetMsgToGroup(INVALID_08);
		SetMsgObjectNumberTo(idToMsg);
		sendToTCP();
	}
	else {
		printf("Stop automat: %u\n", GetObjectId());

		// Postavljamo automat u stanje IDLE i
		// saljemo poruku sledecem automatu
		PrepareNewMessage(0x00, MSG_STOP);
		SetMsgToAutomate(FSM_TYPE_AUTOMAT);
		SetMsgToGroup(INVALID_08);
		SetMsgObjectNumberTo(idToMsg);
		sendToTCP();
		SetState(IDLE);
	}
}

void net_automat::net_automat_MSG_STOP() {
	printf("Stop automat: %u\n", GetObjectId());
	SetState(IDLE);
}

void net_automat::net_automat_UNEXPECTED_IDLE() {
	printf("Neocekivana poruka u stanju IDLE \n");
}

void net_automat::net_automat_UNEXPECTED_MSG() {
	printf("Neocekivana poruka u stanju SEND \n");
}

void net_automat::StartDemo() {
	uint8* msg = GetBuffer(MSG_HEADER_LENGTH);
	SetMsgFromAutomate(FSM_TYPE_AUTOMAT, msg);
	SetMsgFromGroup(INVALID_08, msg);
	SetMsgObjectNumberFrom(0, msg);
	SetMsgToAutomate(FSM_TYPE_AUTOMAT, msg);
	SetMsgToGroup(INVALID_08, msg);
	SetMsgObjectNumberTo(0, msg);
	SetMsgInfoCoding(0, msg); // 0 = StandardMessage
	SetMsgCode(IDLE_START, msg);
	SetMsgInfoLength(0, msg);
	SendMessage(MBX_AUTOMAT_ID, msg);
}

uint16 net_automat::convertNetToFSMMessage() {
	// Ovde je potrebno poslati samo podatke posto se nova
	// poruka salje samom sebi
	int length = receivedMessageLength - MSG_HEADER_LENGTH;
	memcpy(fsmMessageR, protocolMessageR + MSG_HEADER_LENGTH, length);
	fsmMessageRLength = length;//obavezno postaviti jer to koristi
	workWhenReceive(0);
	// Rotiramo bajtove
	uint16 msgCode = GetUint16((uint8*)(protocolMessageR + MSG_CODE));
	switch (msgCode) {
	case IDLE_START:
		msgCode = IDLE_START;
		break;
	case IDLE_MSG:
		msgCode = IDLE_MSG;
		break;
	case MSG_MSG:
		msgCode = MSG_MSG;
		break;
	case MSG_STOP:
		msgCode = MSG_STOP;
		break;
	default:
		msgCode = 0xffff;
	}
	return msgCode;
}

void net_automat::convertFSMToNetMessage() {
	// Ovde se salje cela poruka
	memcpy(protocolMessageS, fsmMessageS, fsmMessageSLength);
	sendMsgLength = fsmMessageSLength;
}

uint8 net_automat::getProtocolInfoCoding() {
	// Standard msg info coding
	return 0;
}