#pragma once

#include "kernelTypes.h"


// FSM IDs
const uint8 SERVER_TYPE_ID = 0x01;
const uint8 CLIENT_TYPE_ID = 0x02;

// FSM message box IDs
const uint8 SERVER_MBX_ID = 0x01;
const uint8 CLIENT_MBX_ID = 0x02;

// Server messages:
const uint16 MSG_User_Send = 0x0001;

// Client messages:
const uint16 MSG_Frame = 0x0002;

// Network constants
#define ADDRESS "127.0.0.1"
#define PORT 5000

// Message parameter defines
#define PARAM_INADDR 0x01
#define PARAM_DATA 0x02
#define PARAM_LEN 0x03
#define PARAM_DST 0x04
