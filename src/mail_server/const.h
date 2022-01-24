#pragma once

// FSM
#define FSM_TYPE_AUTOMAT 0

// MBX
#define MBX_AUTOMAT_ID 0
#define MAX_MSG_NUM 10
#define NUM_AUTOMATA 3
#define COUNT 1
#define PARAM_TEXT 2
#define IP_ADDRES "192.168.1.150"

#define PORT_1 7000
#define PORT_2 8000

enum StanjaAutomata
{
	IDLE = 0,
	MESSAGE,
};

enum Poruke
{
	IDLE_START = 0,
	IDLE_MSG,
	MSG_MSG,
	MSG_STOP
};