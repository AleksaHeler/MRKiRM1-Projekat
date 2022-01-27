#ifndef CONST_H
#define CONST_H


#include <fsm.h>
#include <fsmsystem.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>

#include "kernelTypes.h"
#include "stdMsgpc16pl16.h"
#include "logfile.h"
#include "NetFSM.h"


const bool DEBUG = true;
#define SERVER_DEBUG_NAME "[Server] "
#define CLIENT_DEBUG_NAME "\t\t\t\t\t\t[CLIENT] "


// FSM IDs
const uint8 SERVER_TYPE_ID = 0x00;
const uint8 CLIENT_TYPE_ID = 0x01;

// FSM message box IDs
const uint8 SERVER_MBX_ID = 0x00;
const uint8 CLIENT_MBX_ID = 0x01;

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
#define MAX_STRING_SIZE 64
#define MAX_MAIL_SIZE 1024
#define MAX_MESSAGES_IN_INBOX 64

// Message parameter defines
#define PARAM_MAIL_COUNT 0x01			// For checking inbox, how many mails unread

// Commands sent as string over internet
#define LOGIN_COMMAND "login"
#define LOGOUT_COMMAND "logout"
#define SENDMAIL_COMMAND "send"
#define CHECKMAIL_COMMAND "check"
#define RECEIVEMAIL_COMMAND "receive"
#define MESSAGE_SPLIT_TOKEN "|"


#endif // CONST_H