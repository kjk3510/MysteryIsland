#include <windows.h>
#include <xnamath.h>

#define SERVERIP "127.0.0.1"
#define SERVERPORT 9000
#define WM_SOCKET            WM_USER + 1

<<<<<<< HEAD
#define CS_UP    1
#define CS_DOWN  2
#define CS_LEFT  3
#define CS_RIGHT    4
#define CS_MOUSE	5

#define SC_CON		1
#define SC_POS		2
#define SC_MOUSE	3

#define MAX_BUFF_SIZE   4000
#define MAX_PACKET_SIZE  255

#define MAX_USER 10

#pragma pack (push, 1)

struct cs_packet_player {
	BYTE size;
	BYTE type;
	WORD id;
	BYTE key_state;
	XMFLOAT4X4 world;
};

struct sc_packet_player {
	BYTE size;
	BYTE type;
	WORD id;
	XMFLOAT4X4 world;
};

#pragma pack (pop)
=======
>>>>>>> parent of c6a9d8d... 더미클라 키값 전송확인 / 졸작클라에 붙이는 작업중, 난항겪는중
