#include <windows.h>

#define SERVERIP "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE 512
#define WM_SOCKET            WM_USER + 1

#define CS_UP    1
#define CS_DOWN  2
#define CS_LEFT  3
#define CS_RIGHT    4

#define SC_POS           1
#define SC_MOUSE		 2

#pragma pack (push, 1)

struct cs_packet_front {
	BYTE size;
	BYTE type;
};

struct cs_packet_back {
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

struct cs_packet_mouse {
	BYTE size;
	BYTE type;
};

struct sc_packet_pos {
	BYTE size;
	BYTE type;
	WORD id;
	BYTE x;
	BYTE y;
	BYTE z;
};

struct sc_packet_mouse {
	BYTE size;
	BYTE type;
	WORD id;
	BYTE x;
	BYTE y;
};

#pragma pack (pop)