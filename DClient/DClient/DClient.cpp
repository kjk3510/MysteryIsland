//
//��Ƽ����Ʈ ���� ���� ������� �ٲ���� ��!
//
#pragma comment(lib, "ws2_32")
#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include <list>
#include <thread>
//��ó����

using namespace std;
#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE    1024

//�Ʒ��� ��ŶŸ�� CS->Client to Server, SC->Server to Client
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

// �Ʒ��κ��� ���� ��������.h ���� �� �ܾ�� �����Ƽ�

typedef struct PLAYER {         //int y�߰�
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

// Ŭ���̾�Ʈ�� Ȯ�� ������ ����ü�� ������ �Ѵ�.
typedef struct CLIENT {
	int            id;
	bool         is_connected;
	SOCKET         sock;
	PLAYER         player;
	OverlapEx      recv_overlap;
	int            previous_data_size;
	unsigned char   packet[MAX_BUFF_SIZE];      //������ ��Ŷ���� ���յǴ� ����

}CLIENT;
//////////////////////

//�Ʒ��� ��Ŷ ������
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

struct sc_packet_pos {   //��ġ ��Ŷ
	BYTE size;
	BYTE type;
	WORD id;
	BYTE x;
	BYTE y;
};

struct sc_packet_put_player {   //���� �˸� ��Ŷ
	BYTE size;
	BYTE type;
	WORD id;
	BYTE x;
	BYTE y;
};
struct sc_packet_remove_player { //���� ���� ��Ŷ
	BYTE size;
	BYTE type;
	WORD id;
};

struct sc_packet_chat {         //ä�� ��Ŷ
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

//����������
SOCKET sock; // Ŭ���̾�Ʈ ��� ����

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

	// socket()   ->�񵿱� ���� ����
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

	//EventSelect�� ����ϱ� ���ؼ� �̺�Ʈ �ڵ� �ϳ� ����
	WSAEVENT hEvent = WSACreateEvent();         //#define WSAEVENT HANDLE ���������� �̷��� ������ �� �̰� �ڵ���
	if (hEvent == WSA_INVALID_EVENT)
	{
		err_display("wsacreateevent()");
		return FALSE;
	}

	::WSAEventSelect(sock, hEvent, FD_CLOSE | FD_READ);
	// Ŭ���̾�Ʈ ����(sock)�� �̺�Ʈ �ڵ�(hEvent)�� ��������ְ�
	// Ŭ���̾�Ʈ�� FD_CLOSE, FD_READ �� �ΰ��� ��Ʈ��ũ �̺�Ʈ�� �������
	// FD_CLOSE -> ���� ��������, FD_READ->��Ŷ ������
	// cf.FD_ACCEPT, FD_WRITE�� ������ ���������� �ʿ��ϹǷ� ���ʿ�X



	//�� �Ʒ��κ� ���� �߸𸣰��� ����
	send_wsabuf.buf = send_buffer;
	send_wsabuf.len = BUFSIZE;
	recv_wsabuf.buf = recv_buffer;
	recv_wsabuf.len = BUFSIZE;

	cout << "�ƹ��ų� �Է��ϼ�. " << endl;
	cin >> buf;

	WSANETWORKEVENTS NetworkEvents;      //� �̺�Ʈ�� �Դ��� �����ϱ� ���� ����ü
										 //187�ٿ� ����° ���ڷ� ��. �̺�Ʈ�� �ɸ��� � �̺�Ʈ���� ���⿡ �����̵�
	int networkEvent;               //���� �˻��ϱ� ���� ����
									//1�ʸ��� �޼��� ����
	bool asdf = false;
	while (true)
	{
		networkEvent = ::WSAEnumNetworkEvents(sock, hEvent, &NetworkEvents);
		//���ٿ� �Լ��� ȣ�������ν� ��ü���� ��Ʈ��ũ �̺�Ʈ�� �˾ƿ�. ������ FD_READ,FD_CLOSE �ΰ��� ����� �س���.
		if (networkEvent == SOCKET_ERROR)
			continue;
		//�̺�Ʈ �ɸ��� �Ʒ� if�� ����
		if (NetworkEvents.lNetworkEvents & FD_READ || NetworkEvents.lNetworkEvents &FD_WRITE)
		{
			//������ ��Ʈ��ũ �̺�Ʈ�� �ɸ��� IF���� ����
			//�Ʒ��� ReadPacket()�Լ��� ��
			ReadPacket(sock);
		}

		//�Ʒ��� �ϴ� ��Ŷ �������Բ��� �س���. 
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
	::WSACloseEvent(hEvent);   //�̺�Ʈ�� ���������� �ݾƾ���
	WSACleanup();            //

}
void ReadPacket(SOCKET sock)
{
	DWORD iobyte, ioflag = 0;
	//�� �Լ��� ���ͼ� ������ Recv�ϰ�
	//�ؿ��� �״�� �����
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
			ProcessPacket(packet_buffer);         // �׸��� ���� ��Ŷ���� ���ʿ� ProcessPacket�Լ��� ��Ŷ ó�� �Լ�
												  // ��������.
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
	//static bool first_time = true;      <-�̰� ��������
	//ptr[1]�� ������ ��Ŷ���� �׻� ù��° ������ size���� �ι����� ��Ŷ Ÿ���̾����Ƿ�
	//�Ʒ�ó�� ��Ŷ ó�� ���� �������ָ�ǿ�.
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
		// TODO : �÷��̾� ���� ��Ŷ->���� �κ� ó�� How?
		// 1.������ ���� ���ѹ���
		// 2. ȥ�ڼ� �� �÷���, �ش� �÷��̾ �ٽ� �����Ҷ����� ȥ�� ������.
		cout << "REMOVE PLAYER ��Ŷ" << endl;
		break;
	}
	case MAP_CAVE_STAGE1:
	{
		//TODO : ���� ���������� 1�ܰ�� ����ȯ�� ���ٰ�
		cout << "���� ��1 ����ȯ ��Ŷ" << endl;
		break;
	}
	case MAP_CAVE_STAGE2:
	{
		//TODO : ���� ���������� 2�ܰ�� ����ȯ�� ���ٰ�
		cout << "���� ��2 ����ȯ ��Ŷ" << endl;
		break;
	}
	case MAP_CAVE_STAGE3:
	{
		//TODO : ���� ���������� 3�ܰ�� ����ȯ�� ���ٰ�
		cout << "���� ��3 ����ȯ ��Ŷ" << endl;
		break;
	}
	default:
		printf("Unknown PACKET type [%d]\n", ptr[1]);
	}
}


// ���� �Լ� ���� ��� �� ����
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

// ���� �Լ� ���� ���
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