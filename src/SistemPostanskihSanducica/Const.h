#pragma once

#include "kernelTypes.h"


// FSM IDs
const uint8 SERVER_TYPE_ID = 0x01;
const uint8 CLIENT_TYPE_ID = 0x02;

// FSM message box IDs
const uint8 SERVER_MBX_ID = 0x01;
const uint8 CLIENT_MBX_ID = 0x02;


// Server messages:
const uint16 ServerMSG_LoginOk				= 0x0001;
const uint16 ServerMSG_LoginError			= 0x0002;
const uint16 ServerMSG_LogoutOk				= 0x0003;
const uint16 ServerMSG_CheckMailResponse	= 0x0004;
const uint16 ServerMSG_ReceiveMailResponse	= 0x0005;
const uint16 ServerMSG_ReceiveMailError		= 0x0006;

// Client messages:
const uint16 ClientMSG_Login			= 0x0101;
const uint16 ClientMSG_Logout			= 0x0102;
const uint16 ClientMSG_SendMail			= 0x0103;
const uint16 ClientMSG_CheckMail		= 0x0104;
const uint16 ClientMSG_ReceiveMail		= 0x0105;


// Network constants
#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 5000
#define BUFFER_SIZE 1024

// Message parameter defines
#define PARAM_USERNAME 0x01				// Login data
#define PARAM_PASSWORD 0x02
#define PARAM_MAIL_COUNT 0x04			// For checking inbox, how many mails unread
#define PARAM_MESSAGE_SUBJECT 0x11		// For receiving/sending mail, title and content
#define PARAM_MESSAGE_TEXT 0x12
