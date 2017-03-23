#pragma comment (lib, "ws2_32.lib")
#include <WinSock2.h>
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include "Protocol.h"

#define WindowFullWidth 1000
#define WindowFullHeight 800

// 소켓 정보 저장을 위한 구조체
struct SOCKETINFO
{
	OVERLAPPED overlapped;
	SOCKET sock;
	char buf[BUFSIZE + 1];
	int recvbytes;
	int sendbytes;
	WSABUF wsabuf;
};

WSABUF send_buf;
char send_buffer[BUFSIZE];
WSABUF recv_buf;
char recv_buffer[BUFSIZE];
char packet_buffer[BUFSIZ];

HWND handle = NULL; // 윈도우핸들
HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("Window Class Name");

DWORD      in_packet_size = 0;
int      saved_packet_size = 0;
int g_myid;


SOCKET sock;

// 오류 출력 함수
void err_quit(char *msg);
void err_display(char *msg);
void clienterror();

// 패킷작업함수
void ReadPacket(SOCKET sock);
void ProcessPacket(char *ptr);

// 작업자 스레드 함수
DWORD WINAPI ClientMain(LPVOID arg);

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

using namespace std;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;
	// 윈도우 클래스 구조체 값 설정
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;

	// 윈도우 클래스 등록
	RegisterClass(&WndClass);
	// 윈도우 생성
	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW, 0, 0,
		800, 600, NULL, (HMENU)NULL, hInstance, NULL);

	// 윈도우 출력
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	handle = hWnd;

	SOCKET sock;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// WSASocket(주소체계, 소켓타입, 프로토콜, 프로토콜정보, 몰라, 몰라) <-> socket(주소체계, 소켓타입, 프로토콜)
	sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// WSAConnect(소켓, 서버주소, 주소크기, 몰라, 몰라, 몰라, 몰라) <-> connet(소켓, 서버주소, 주소크기)
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	int retval = WSAConnect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr), NULL, NULL, NULL, NULL);

	// 클라이언트는 WSAAsyncSelect 모델을 사용하는게 좋다고 들음. 왜였지?
	WSAAsyncSelect(sock, handle, WM_SOCKET, FD_CLOSE | FD_READ);

	// 송수신버퍼 등록
	send_buf.buf = send_buffer;
	send_buf.len = BUFSIZE;
	recv_buf.buf = recv_buffer;
	recv_buf.len = BUFSIZE;

//	return true;

	// 이벤트 루프 처리
	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	HPEN hPen, oldPen;
	HBRUSH hBrush, oldBrush;

	static int Fx, Fy, Lx, Ly;

	// 메시지 처리하기
	switch (uMsg) {
	case WM_CREATE:
		AllocConsole();	//콘솔 할당
		FILE *acStreamOut;
		FILE *acStreamIn;
		freopen_s(&acStreamOut, "CONOUT$", "wt", stdout);
		freopen_s(&acStreamIn, "CONIN$", "r", stdin);
		Fx = 100, Fy = 100, Lx = 200, Ly = 200;
		return 0;

	case WM_KEYDOWN:
		if (wParam == VK_UP) {
			Fy = Fy - 100;
			Ly = Ly - 100;
			if (Fy < 0) {
				Fy = 0;
				Ly = 100;
			}
			cout << Fy << " " << Ly << endl;
		}
		else if (wParam == VK_DOWN) {
			Fy = Fy + 100;
			Ly = Ly + 100;
			if (Ly > 400) {
				Fy = 300;
				Ly = 400;
			}
			cout << Fy << " " << Ly << endl;
		}
		else if (wParam == VK_RIGHT) {
			Fx = Fx + 100;
			Lx = Lx + 100;
			if (Lx > 400) {
				Fx = 300;
				Lx = 400;
			}
			cout << Fx << " " << Lx << endl;
		}
		else if (wParam == VK_LEFT) {
			Fx = Fx - 100;
			Lx = Lx - 100;
			if (Fx < 0) {
				Fx = 0;
				Lx = 100;
			}
			cout << Fx << " " << Lx << endl;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		//		hBrush = CreateSolidBrush(RGB(255, 0, 0));
		//		oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
		/*hPen = CreatePen(PS_SOLID, 5, RGB(0, 0, 255));
		oldPen = (HPEN)SelectObject(hdc, hPen);*/
		if (WM_KEYDOWN) {
			hBrush = CreateSolidBrush(RGB(255, 0, 0));
			oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
			Rectangle(hdc, Fx, Fy, Lx, Ly);
			SelectObject(hdc, oldBrush);
			DeleteObject(hBrush);
		}
		else {
			hPen = CreatePen(PS_SOLID, 5, RGB(0, 0, 255));
			oldPen = (HPEN)SelectObject(hdc, hPen);
			Rectangle(hdc, Fx, Fy, Lx, Ly);
			SelectObject(hdc, oldPen);
			DeleteObject(hPen);
		}
		/*Rectangle(hdc, Fx, Fy, Lx, Ly);
		SelectObject(hdc, oldPen);
		DeleteObject(hPen);*/
		//		SelectObject(hdc, oldBrush);
		//		DeleteObject(hBrush);
		hPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
		oldPen = (HPEN)SelectObject(hdc, hPen);
		{
			MoveToEx(hdc, 0, 0, NULL);
			LineTo(hdc, 400, 0);

			MoveToEx(hdc, 0, 100, NULL);
			LineTo(hdc, 400, 100);

			MoveToEx(hdc, 0, 200, NULL);
			LineTo(hdc, 400, 200);

			MoveToEx(hdc, 0, 300, NULL);
			LineTo(hdc, 400, 300);

			MoveToEx(hdc, 0, 400, NULL);
			LineTo(hdc, 400, 400);

			MoveToEx(hdc, 0, 0, NULL);
			LineTo(hdc, 0, 400);

			MoveToEx(hdc, 100, 0, NULL);
			LineTo(hdc, 100, 400);

			MoveToEx(hdc, 200, 0, NULL);
			LineTo(hdc, 200, 400);

			MoveToEx(hdc, 300, 0, NULL);
			LineTo(hdc, 300, 400);

			MoveToEx(hdc, 400, 0, NULL);
			LineTo(hdc, 400, 400);
		}
		SelectObject(hdc, oldPen);
		DeleteObject(hPen);
		EndPaint(hWnd, &ps);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		FreeConsole();   //콘솔을 해제	
		return 0;

	case WM_SOCKET:{
		std::cout << "WM_SOCKET 되는가" << std::endl;
		if (WSAGETSELECTERROR(lParam)) {
		closesocket((SOCKET)wParam);
		clienterror();
		break;
		}
		switch (WSAGETSELECTEVENT(lParam)) {
		case FD_READ:
		std::cout << "리드이벤트 메시지 받나" << std::endl;
		ReadPacket((SOCKET)wParam);
		std::cout << "리드패킷함수 읽나" << std::endl;
		break;
		case FD_CLOSE:
		closesocket((SOCKET)wParam);
		clienterror();
		break;
		}
	  }
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam); // 나머지는 OS로
}

DWORD WINAPI ClientMain(LPVOID arg)
{
	return 0;
}

void clienterror()
{
	exit(-1);
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
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

void ProcessPacket(char *ptr)
{
	static bool first_time = true;
	switch (ptr[1])
	{
	case SC_PUT_PLAYER:
	{
		cout << "asdf" << endl;
		break;
	}
	default:
		printf("Unknown PACKET type [%d]\n", ptr[1]);
	}
}

void ReadPacket(SOCKET sock)
{
	DWORD iobyte, ioflag = 0;
	int retval = WSARecv(sock, &recv_buf, 1, &iobyte, &ioflag, NULL, NULL);
	if (retval) {
		int err_code = WSAGetLastError();
		printf("Recv Error [%d]\n", err_code);
	}

	BYTE *ptr = reinterpret_cast<BYTE *>(recv_buffer);

	while (0 != iobyte) {
		if (0 == in_packet_size) in_packet_size = ptr[0];
		if (iobyte + saved_packet_size >= in_packet_size) {
			memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
			ProcessPacket(packet_buffer);
			ptr += in_packet_size - saved_packet_size;
			iobyte -= in_packet_size - saved_packet_size;
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else {
			memcpy(packet_buffer + saved_packet_size, ptr, iobyte);
			saved_packet_size += iobyte;
			iobyte = 0;
		}
	}
}