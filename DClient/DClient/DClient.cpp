//
//멀티바이트 문자 집합 사용으로 바꿔줘야 함!
//
#pragma comment(lib, "ws2_32")
#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include <list>
#include <thread>
//전처리기

using namespace std;
#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE    1024

//아래는 패킷타입 CS->Client to Server, SC->Server to Client
#define CS_UP               1
#define CS_DOWN               2
#define CS_LEFT               3
#define CS_RIGHT            4
#define CS_CHAT               5

//BufSize
#define MAX_BUFF_SIZE         4000
#define MAX_PACKET_SIZE         255
#define   MAX_STR_SIZE         50

#define SC_POS               1
#define SC_PUT_PLAYER         2
#define SC_REMOVE_PLAYER      3

#define MAP_CAVE            100
#define MAP_CAVE_STAGE1         101
#define MAP_CAVE_STAGE2         102
#define MAP_CAVE_STAGE3         103
#define MAP_CAVE_STAGE1_CLEAR   104
#define MAP_CAVE_STAGE2_CLEAR   105
#define MAP_CAVE_STAGE3_CLEAR   106

// 아랫부분은 서버 프로토콜.h 에서 걍 긁어옴 귀찮아서

typedef struct PLAYER {         //int y추가
	int         x;
	int         z;
	list<int>   viewList;
} PLAYER;

typedef struct OverlapEx
{
	WSAOVERLAPPED   original_overlap;
	int            operation;
	WSABUF         recv_buff;
	unsigned char   socket_buff[MAX_BUFF_SIZE];
	int            packet_size;

}OverlapEx;

// 클라이언트는 확장 오버랩 구조체를 가져야 한다.
typedef struct CLIENT {
	int            id;
	bool         is_connected;
	SOCKET         sock;
	PLAYER         player;
	OverlapEx      recv_overlap;
	int            previous_data_size;
	unsigned char   packet[MAX_BUFF_SIZE];      //조각난 패킷들이 조합되는 공간

}CLIENT;
//////////////////////

//아래는 패킷 종류들
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

struct cs_packet_chat {
	BYTE size;
	BYTE type;
	WCHAR message[MAX_STR_SIZE];
};

struct sc_packet_pos {   //위치 패킷
	BYTE size;
	BYTE type;
	WORD id;
	BYTE x;
	BYTE y;
};

struct sc_packet_put_player {   //접속 알림 패킷
	BYTE size;
	BYTE type;
	WORD id;
	BYTE x;
	BYTE y;
};
struct sc_packet_remove_player { //접속 종료 패킷
	BYTE size;
	BYTE type;
	WORD id;
};

struct sc_packet_chat {         //채팅 패킷
	BYTE size;
	BYTE type;
	WORD id;
	WCHAR message[MAX_STR_SIZE];
};

struct cs_packet_map {
	BYTE size;
	BYTE type;
};

void err_quit(char *msg);
void err_display(char *msg);

//전역변수들
SOCKET sock; // 클라이언트 통신 소켓

WSABUF   send_wsabuf;
char    send_buffer[BUFSIZE];
WSABUF   recv_wsabuf;
char   recv_buffer[BUFSIZE];

char   packet_buffer[BUFSIZE];
DWORD      in_packet_size = 0;
int      saved_packet_size = 0;

char buf[50];
char recv_buf[50];
void ProcessPacket(char *ptr);
void ReadPacket(SOCKET sock);

int main()
{
	int retval;      //return value
	DWORD iobyte = 0;
	DWORD iobyte2, ioflag = 0;
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()   ->비동기 소켓 생성
	sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
	if (sock == INVALID_SOCKET)
		err_quit("socket()");

	SOCKADDR_IN ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(SOCKADDR_IN));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(SERVERPORT);
	ServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	//Connect
	int Result = WSAConnect(sock, (sockaddr *)&ServerAddr, sizeof(ServerAddr), NULL, NULL, NULL, NULL);
	if (Result == SOCKET_ERROR)
		err_quit("wsa_connect");

	//EventSelect를 사용하기 위해서 이벤트 핸들 하나 생성
	WSAEVENT hEvent = WSACreateEvent();         //#define WSAEVENT HANDLE 내부적으로 이렇게 되있음 걍 이거 핸들임
	if (hEvent == WSA_INVALID_EVENT)
	{
		err_display("wsacreateevent()");
		return FALSE;
	}

	::WSAEventSelect(sock, hEvent, FD_CLOSE | FD_READ);
	// 클라이언트 소켓(sock)을 이벤트 핸들(hEvent)에 연결시켜주고
	// 클라이언트는 FD_CLOSE, FD_READ 이 두개의 네트워크 이벤트만 있으면됨
	// FD_CLOSE -> 서버 끊겼을때, FD_READ->패킷 왔을때
	// cf.FD_ACCEPT, FD_WRITE등 있으나 서버에서나 필요하므로 알필요X



	//이 아랫부분 봐도 잘모르겠음 아직
	send_wsabuf.buf = send_buffer;
	send_wsabuf.len = BUFSIZE;
	recv_wsabuf.buf = recv_buffer;
	recv_wsabuf.len = BUFSIZE;

	cout << "아무거나 입력하셈. " << endl;
	cin >> buf;

	WSANETWORKEVENTS NetworkEvents;      //어떤 이벤트가 왔는지 저장하기 위한 구조체
										 //187줄에 세번째 인자로 들어감. 이벤트가 걸리면 어떤 이벤트인지 여기에 저장이됨
	int networkEvent;               //오류 검사하기 위한 변수
									//1초마다 메세지 전송
	bool asdf = false;
	while (true)
	{
		networkEvent = ::WSAEnumNetworkEvents(sock, hEvent, &NetworkEvents);
		//윗줄에 함수를 호출함으로써 구체적인 네트워크 이벤트를 알아옴. 위에서 FD_READ,FD_CLOSE 두개만 등록을 해놨음.
		if (networkEvent == SOCKET_ERROR)
			continue;
		//이벤트 걸리면 아래 if문 실행
		if (NetworkEvents.lNetworkEvents & FD_READ || NetworkEvents.lNetworkEvents &FD_WRITE)
		{
			//소켓이 네트워크 이벤트에 걸리면 IF문에 들어가서
			//아래에 ReadPacket()함수로 들어감
			ReadPacket(sock);
		}

		//아래는 일단 패킷 보내놓게끔만 해놓음. 
		/*cs_packet_up *my_packet = reinterpret_cast<cs_packet_up *>(send_buffer);
		my_packet->type = CS_LEFT;
		my_packet->size = sizeof(my_packet);
		send_wsabuf.len = sizeof(my_packet);*/
		//MAP_PACKET
		if (!asdf)
		{
			cs_packet_map * map_packet = reinterpret_cast<cs_packet_map *>(send_buffer);
			map_packet->type = CS_UP;
			map_packet->size = sizeof(map_packet);
			send_wsabuf.len = sizeof(map_packet);


			retval = WSASend(sock, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
			asdf = true;
		}
		//retval = send(sock, reinterpret_cast<char*>(myPacket), sizeof(buf), 0);

		Sleep(1000);
	}
	::WSACloseEvent(hEvent);   //이벤트를 생성했으니 닫아야함
	WSACleanup();            //

}
void ReadPacket(SOCKET sock)
{
	DWORD iobyte, ioflag = 0;
	//이 함수에 들어와서 데이터 Recv하고
	//밑에꺼 그대로 쓰면됨
	int ret = WSARecv(sock, &recv_wsabuf, 1, &iobyte, &ioflag, NULL, NULL);
	if (ret)
	{
		int err_code = WSAGetLastError();
		printf("Recv Error [%d]\n", err_code);
	}

	BYTE *ptr = reinterpret_cast<BYTE *>(recv_buffer);

	while (0 != iobyte)
	{
		if (0 == in_packet_size)
			in_packet_size = ptr[0];
		if (iobyte + saved_packet_size >= in_packet_size)
		{
			memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
			ProcessPacket(packet_buffer);         // 그리고 받은 패킷으로 왼쪽에 ProcessPacket함수로 패킷 처리 함수
												  // 실행해줌.
			ptr += in_packet_size - saved_packet_size;
			iobyte -= in_packet_size - saved_packet_size;
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else
		{
			memcpy(packet_buffer + saved_packet_size, ptr, iobyte);
			saved_packet_size += iobyte;
			iobyte = 0;
		}
	}
}

void ProcessPacket(char *ptr)
{
	//static bool first_time = true;      <-이거 뭔지몰라
	//ptr[1]인 이유는 패킷들은 항상 첫번째 변수가 size였고 두번쨰가 패킷 타입이었으므로
	//아랫처럼 패킷 처리 구문 생성해주면되요.
	switch (ptr[1])
	{
	case SC_PUT_PLAYER:
	{
		//TODO : 
		cout << "put_player_packet" << endl;
		break;
	}
	case SC_POS:
	{
		/*sc_packet_pos *my_packet = reinterpret_cast<sc_packet_pos *>(ptr);
		int other_id = my_packet->id;
		if (other_id == g_myid) {
		g_left_x = my_packet->x - 4;
		g_top_y = my_packet->y - 4;
		player.x = my_packet->x;
		player.y = my_packet->y;
		}
		else if (other_id < NPC_START) {
		skelaton[other_id].x = my_packet->x;
		skelaton[other_id].y = my_packet->y;
		}
		else {
		npc[other_id - NPC_START].x = my_packet->x;
		npc[other_id - NPC_START].y = my_packet->y;
		}*/
		cout << "SC_POS" << endl;
		break;
	}
	case SC_REMOVE_PLAYER:
	{
		// TODO : 플레이어 끊김 패킷->종료 부분 처리 How?
		// 1.게임을 종료 시켜버림
		// 2. 혼자서 겜 플레이, 해당 플레이어가 다시 접속할때까지 혼자 겜진행.
		cout << "REMOVE PLAYER 패킷" << endl;
		break;
	}
	case MAP_CAVE_STAGE1:
	{
		//TODO : 동굴 스테이지의 1단계로 씬전환을 해줄것
		cout << "동굴 맵1 씬전환 패킷" << endl;
		break;
	}
	case MAP_CAVE_STAGE2:
	{
		//TODO : 동굴 스테이지의 2단계로 씬전환을 해줄것
		cout << "동굴 맵2 씬전환 패킷" << endl;
		break;
	}
	case MAP_CAVE_STAGE3:
	{
		//TODO : 동굴 스테이지의 3단계로 씬전환을 해줄것
		cout << "동굴 맵3 씬전환 패킷" << endl;
		break;
	}
	default:
		printf("Unknown PACKET type [%d]\n", ptr[1]);
	}
}


// 소켓 함수 오류 출력 후 종료
void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, TEXT("msg"), MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	LocalFree(lpMsgBuf);
}