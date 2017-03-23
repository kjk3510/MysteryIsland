// LabProject02.cpp : Defines the entry point for the application.
//


#include "stdafx.h"
#include "LabProject11-0.h"
#include "GameFramework.h"
#include "..\Server_iocp\Server_iocp\Protocol.h"

#pragma comment (lib, "ws2_32.lib")
#include <WinSock2.h>
#include <windows.h>
#include <stdio.h>
#include <iostream>

#define WM_SOCKET		WM_USER + 1
#define SERVERIP "127.0.0.1"

WSABUF send_buf;
char send_buffer[BUFSIZ];
WSABUF recv_buf;
char recv_buffer[BUFSIZ];
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

//게임 프로그램의 골격을 나타내는 객체
CGameFramework gGameFramework;
//게임 프로그램의 골격을 나타내는 객체

#pragma warning(disable:4996)

#define MAX_LOADSTRING 100

// Global Variables:
//HINSTANCE ghInstance;
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	HWND hWnd;
	MSG msg;
	HACCEL hAccelTable;

	AllocConsole();
	freopen("CONOUT$", "wt", stdout);

	srand((unsigned)time(NULL));

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_LABPROJECT110, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LABPROJECT110));

	// Main message loop:

	//	handle = hWnd;

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
	send_buf.len = BUFSIZ;
	recv_buf.buf = recv_buffer;
	recv_buf.len = BUFSIZ;

	while (1)
	{
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;
			if (!::TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
		}
		else
		{
			gGameFramework.FrameAdvance();
		}
	}
	gGameFramework.OnDestroy();

////	handle = hWnd;
//
//	SOCKET sock;
//
//	// 윈속 초기화
//	WSADATA wsa;
//	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
//		return 1;
//
//	// WSASocket(주소체계, 소켓타입, 프로토콜, 프로토콜정보, 몰라, 몰라) <-> socket(주소체계, 소켓타입, 프로토콜)
//	sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
//	if (sock == INVALID_SOCKET) err_quit("socket()");
//
//	// WSAConnect(소켓, 서버주소, 주소크기, 몰라, 몰라, 몰라, 몰라) <-> connet(소켓, 서버주소, 주소크기)
//	SOCKADDR_IN serveraddr;
//	ZeroMemory(&serveraddr, sizeof(serveraddr));
//	serveraddr.sin_family = AF_INET;
//	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
//	serveraddr.sin_port = htons(SERVERPORT);
//	int retval = WSAConnect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr), NULL, NULL, NULL, NULL);
//
//	// 클라이언트는 WSAAsyncSelect 모델을 사용하는게 좋다고 들음. 왜였지?
//	WSAAsyncSelect(sock, handle, WM_SOCKET, FD_CLOSE | FD_READ);
//
//	// 송수신버퍼 등록
//	send_buf.buf = send_buffer;
//	send_buf.len = BUFSIZ;
//	recv_buf.buf = recv_buffer;
//	recv_buf.len = BUFSIZ;

	return (int)msg.wParam;
}
/*
응용 프로그램 마법사가 생성한 코드의 메시지 루프는 응용 프로그램이 처리해야 할 윈도우
메시지가 메시지 큐에 있으면 꺼내와서 처리를 하고 메시지가 없으면 CPU를 운영체ㅈ로 반납
하도록 되어있다(GETMessage() )그러나 게임 프로그램은 프로그램이 처리할 메시지
가 없더라도 화면 렌더링, 사용자 입력 처리, 길 찾기 등의 작업이 계속 진행되어야 한다. 그러
므로 만약 처리할 멧지가 없더라도 CPU를 반납하지 않고 게임이 계속 진행되도록 해야한다.
이를 위해서 윈도우 메시지 루프를 PeekMessage() API함수를 사용하여 변경한다.
peekMessage() API함수는 메시지 큐를 살펴보고 메시지가 있으면 메세지를 꺼내고 TRUE를
반환한다 만약 메세지 큐에 메시지가 없으면 False를 반환한다. 그러므로 peekmessage()함수가 TRue를
반환하는 경우(응용 프로그램이 처리해야 할 윈도우 메시지가 메시지 큐에 있으면)
정상적인 윈도우 멧지ㅣ 처리 과정을 수행해야 한다. 그러나 false를 봔한하느 ㄴ경우(메시지 큐가 비어
있으면)gGameFramework.frameAdvace()함수를 호출하여 게임 프로그램이 CPU를 사용할 수 있도록 해야한다.
그리고 이 과정은 사용자가 프로그램을 종료할 때 까지 반복되도록 한다.
그리고 메시지 루프가 종료되면 gGameFramework.Ondestroy()함수를 호출하여 프레임워크 객체를 소멸하도록 한다.
*/


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDC_LABPROJECT110));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_LABPROJECT110);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance;

	RECT rc = { 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };
	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER;
	AdjustWindowRect(&rc, dwStyle, FALSE);
	HWND hMainWnd = CreateWindow(szWindowClass, szTitle, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL);
	if (!hMainWnd) return(FALSE);

	gGameFramework.OnCreate(hInstance, hMainWnd);

	::ShowWindow(hMainWnd, nCmdShow);
	::UpdateWindow(hMainWnd);

	return(TRUE);
}
/*
프로그램의 주 윈도우가 생성되면 CGameFramework클래스의 Oncreate()함수를 호출하여 프레임워크 객체를
초기화하도록 한다.

*/

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent, wmSEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	/*
	WndProc()함수의 메시지 처리 부분을 다음과 같이 변경한다.
	게임 프레임워크에서 처리할 메시지가 있으면 case문장을 추가한다.

	*/
	switch (message)
	{
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_KEYDOWN:
	case WM_KEYUP:
		gGameFramework.OnProcessingWindowMessage(hWnd, message, wParam, lParam);
		break;
	/*case WM_SOCKET: {
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
	}*/
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	case WM_SOCKET: {
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
	return (INT_PTR)FALSE;
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
	cout << "리드패킷함수호출" << endl;
	DWORD iobyte, ioflag = 0;
	int retval = WSARecv(sock, &recv_buf, 1, &iobyte, &ioflag, NULL, NULL);
	if (retval) {
		int err_code = WSAGetLastError();
		printf("Recv Error [%d]\n", err_code);
	}

	cout << "리시브완료" << endl;

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
			cout << "패킷분석함수완료" << endl;
		}
		else {
			memcpy(packet_buffer + saved_packet_size, ptr, iobyte);
			saved_packet_size += iobyte;
			iobyte = 0;
		}
	}
}