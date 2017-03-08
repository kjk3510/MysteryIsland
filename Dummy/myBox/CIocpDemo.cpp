#pragma comment (lib, "ws2_32.lib")
#include <WinSock2.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <Windows.h>

#define SERVERIP "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE 512
#define WM_SOCKET            WM_USER + 1

// 오류 출력 함수
void err_quit(char *msg);
void err_display(char *msg);

// 소켓 통신 스레드 함수
DWORD WINAPI ClientMain(LPVOID arg);

HWND handle = NULL; // 윈도우핸들
SOCKET sock; // 소켓
char buf[BUFSIZE + 1]; // 데이터 송수신 버퍼
					  
// 소켓 함수 오류 출력 후 종료
void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
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

DWORD WINAPI ClientMain(LPVOID arg)
{
	int retval;

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
	retval = WSAConnect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr), NULL, NULL, NULL, NULL);

	// 클라이언트는 WSAAsyncSelect 모델을 사용하는게 좋다고 들음. 왜였지?
	WSAAsyncSelect(sock, handle, WM_SOCKET, FD_CLOSE | FD_READ)

	return 0;
}