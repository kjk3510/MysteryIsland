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

HWND handle = NULL; // �������ڵ�
HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("Window Class Name");

DWORD      in_packet_size = 0;
int      saved_packet_size = 0;
int g_myid;


SOCKET sock;

// ���� ��� �Լ�
void err_quit(char *msg);
void err_display(char *msg);
void clienterror();

// ��Ŷ�۾��Լ�
void ReadPacket(SOCKET sock);
void ProcessPacket(char *ptr);

//���� ���α׷��� ����� ��Ÿ���� ��ü
CGameFramework gGameFramework;
//���� ���α׷��� ����� ��Ÿ���� ��ü

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

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// WSASocket(�ּ�ü��, ����Ÿ��, ��������, ������������, ����, ����) <-> socket(�ּ�ü��, ����Ÿ��, ��������)
	sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// WSAConnect(����, �����ּ�, �ּ�ũ��, ����, ����, ����, ����) <-> connet(����, �����ּ�, �ּ�ũ��)
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	int retval = WSAConnect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr), NULL, NULL, NULL, NULL);

	// Ŭ���̾�Ʈ�� WSAAsyncSelect ���� ����ϴ°� ���ٰ� ����. �ֿ���?
	WSAAsyncSelect(sock, handle, WM_SOCKET, FD_CLOSE | FD_READ);

	// �ۼ��Ź��� ���
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
//	// ���� �ʱ�ȭ
//	WSADATA wsa;
//	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
//		return 1;
//
//	// WSASocket(�ּ�ü��, ����Ÿ��, ��������, ������������, ����, ����) <-> socket(�ּ�ü��, ����Ÿ��, ��������)
//	sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
//	if (sock == INVALID_SOCKET) err_quit("socket()");
//
//	// WSAConnect(����, �����ּ�, �ּ�ũ��, ����, ����, ����, ����) <-> connet(����, �����ּ�, �ּ�ũ��)
//	SOCKADDR_IN serveraddr;
//	ZeroMemory(&serveraddr, sizeof(serveraddr));
//	serveraddr.sin_family = AF_INET;
//	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
//	serveraddr.sin_port = htons(SERVERPORT);
//	int retval = WSAConnect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr), NULL, NULL, NULL, NULL);
//
//	// Ŭ���̾�Ʈ�� WSAAsyncSelect ���� ����ϴ°� ���ٰ� ����. �ֿ���?
//	WSAAsyncSelect(sock, handle, WM_SOCKET, FD_CLOSE | FD_READ);
//
//	// �ۼ��Ź��� ���
//	send_buf.buf = send_buffer;
//	send_buf.len = BUFSIZ;
//	recv_buf.buf = recv_buffer;
//	recv_buf.len = BUFSIZ;

	return (int)msg.wParam;
}
/*
���� ���α׷� �����簡 ������ �ڵ��� �޽��� ������ ���� ���α׷��� ó���ؾ� �� ������
�޽����� �޽��� ť�� ������ �����ͼ� ó���� �ϰ� �޽����� ������ CPU�� �ü���� �ݳ�
�ϵ��� �Ǿ��ִ�(GETMessage() )�׷��� ���� ���α׷��� ���α׷��� ó���� �޽���
�� ������ ȭ�� ������, ����� �Է� ó��, �� ã�� ���� �۾��� ��� ����Ǿ�� �Ѵ�. �׷�
�Ƿ� ���� ó���� ������ ������ CPU�� �ݳ����� �ʰ� ������ ��� ����ǵ��� �ؾ��Ѵ�.
�̸� ���ؼ� ������ �޽��� ������ PeekMessage() API�Լ��� ����Ͽ� �����Ѵ�.
peekMessage() API�Լ��� �޽��� ť�� ���캸�� �޽����� ������ �޼����� ������ TRUE��
��ȯ�Ѵ� ���� �޼��� ť�� �޽����� ������ False�� ��ȯ�Ѵ�. �׷��Ƿ� peekmessage()�Լ��� TRue��
��ȯ�ϴ� ���(���� ���α׷��� ó���ؾ� �� ������ �޽����� �޽��� ť�� ������)
�������� ������ ������ ó�� ������ �����ؾ� �Ѵ�. �׷��� false�� �����ϴ� �����(�޽��� ť�� ���
������)gGameFramework.frameAdvace()�Լ��� ȣ���Ͽ� ���� ���α׷��� CPU�� ����� �� �ֵ��� �ؾ��Ѵ�.
�׸��� �� ������ ����ڰ� ���α׷��� ������ �� ���� �ݺ��ǵ��� �Ѵ�.
�׸��� �޽��� ������ ����Ǹ� gGameFramework.Ondestroy()�Լ��� ȣ���Ͽ� �����ӿ�ũ ��ü�� �Ҹ��ϵ��� �Ѵ�.
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
���α׷��� �� �����찡 �����Ǹ� CGameFrameworkŬ������ Oncreate()�Լ��� ȣ���Ͽ� �����ӿ�ũ ��ü��
�ʱ�ȭ�ϵ��� �Ѵ�.

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
	WndProc()�Լ��� �޽��� ó�� �κ��� ������ ���� �����Ѵ�.
	���� �����ӿ�ũ���� ó���� �޽����� ������ case������ �߰��Ѵ�.

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
		std::cout << "WM_SOCKET �Ǵ°�" << std::endl;
		if (WSAGETSELECTERROR(lParam)) {
			closesocket((SOCKET)wParam);
			clienterror();
			break;
		}
		switch (WSAGETSELECTEVENT(lParam)) {
		case FD_READ:
			std::cout << "�����̺�Ʈ �޽��� �޳�" << std::endl;
			ReadPacket((SOCKET)wParam);
			std::cout << "������Ŷ�Լ� �г�" << std::endl;
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
		std::cout << "WM_SOCKET �Ǵ°�" << std::endl;
		if (WSAGETSELECTERROR(lParam)) {
			closesocket((SOCKET)wParam);
			clienterror();
			break;
		}
		switch (WSAGETSELECTEVENT(lParam)) {
		case FD_READ:
			std::cout << "�����̺�Ʈ �޽��� �޳�" << std::endl;
			ReadPacket((SOCKET)wParam);
			std::cout << "������Ŷ�Լ� �г�" << std::endl;
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
	cout << "������Ŷ�Լ�ȣ��" << endl;
	DWORD iobyte, ioflag = 0;
	int retval = WSARecv(sock, &recv_buf, 1, &iobyte, &ioflag, NULL, NULL);
	if (retval) {
		int err_code = WSAGetLastError();
		printf("Recv Error [%d]\n", err_code);
	}

	cout << "���ú�Ϸ�" << endl;

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
			cout << "��Ŷ�м��Լ��Ϸ�" << endl;
		}
		else {
			memcpy(packet_buffer + saved_packet_size, ptr, iobyte);
			saved_packet_size += iobyte;
			iobyte = 0;
		}
	}
}