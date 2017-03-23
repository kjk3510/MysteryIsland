#include <Windows.h>

#define SERVERPORT 9000
#define SERVERIP "127.0.0.1"

#define MAX_BUFF_SIZE   4000
#define MAX_PACKET_SIZE  255
#define BUFSIZE			1024

#define WM_SOCKET            WM_USER + 1

#define BOARD_WIDTH   400
#define BOARD_HEIGHT  400

#define VIEW_RADIUS   3

#define MAX_USER 500

#define NPC_START  1000
#define NUM_OF_NPC  10000

#define MAX_STR_SIZE  100

#define CS_UP    1
#define CS_DOWN  2
#define CS_LEFT  3
#define CS_RIGHT    4
#define CS_CHAT		5
#define CS_INPUT	6

#define SC_POS           1
#define SC_PUT_PLAYER    2
#define SC_REMOVE_PLAYER 3
#define SC_CHAT		4
#define SC_CONNECT_RESULT 5

#pragma pack (push, 1)

struct cs_packet_up {
	BYTE size;
	BYTE type;
};

struct cs_packet_down {
	BYTE size;
	BYTE type;
};

struct cs_packet_left {
	BYTE size;
	BYTE type;
};

struct cs_packet_right {
	BYTE size;
	BYTE type;
};
struct cs_packet_DB
{
	BYTE size;
	BYTE type;
	BYTE id;
};

struct cs_packet_chat {
	BYTE size;
	BYTE type;
	WCHAR message[MAX_STR_SIZE];
};

struct sc_packet_DB_message
{
	BYTE size;
	BYTE type;
	WORD id;

};
struct sc_packet_pos {
	BYTE size;
	BYTE type;
	WORD id;
	BYTE x;
	BYTE y;
};

struct sc_packet_put_player {
	BYTE size;
	BYTE type;
	WORD id;
	BYTE x;
	BYTE y;
};
struct sc_packet_remove_player {
	BYTE size;
	BYTE type;
	WORD id;
};

struct sc_packet_chat {
	BYTE size;
	BYTE type;
	WORD id;
	WCHAR message[MAX_STR_SIZE];
};

#pragma pack (pop)

//#include <windows.h>
//#include <xnamath.h>
//
//#define SERVERIP "127.0.0.1"
//#define SERVERPORT 9000
//#define BUFSIZE 512
//#define WM_SOCKET            WM_USER + 1
//
//#define CS_UP    1
//#define CS_DOWN  2
//#define CS_LEFT  3
//#define CS_RIGHT    4
//#define CS_MOUSE	5
//
//#define SC_CON		1
//#define SC_POS		2
//#define SC_MOUSE	3
//
//#pragma pack (push, 1)
//
//struct cs_packet_player {
//	BYTE size;
//	BYTE type;
//	WORD id;
//	BYTE key_state;
//	XMFLOAT4X4 world;
//};
//
//struct sc_packet_player {
//	BYTE size;
//	BYTE type;
//	WORD id;
//	XMFLOAT4X4 world;
//};
//
//#pragma pack (pop)
