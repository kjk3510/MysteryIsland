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

// ���� ��� �Լ�
void err_quit(char *msg);
void err_display(char *msg);

// ���� ��� ������ �Լ�
DWORD WINAPI ClientMain(LPVOID arg);

HWND handle = NULL; // �������ڵ�
SOCKET sock; // ����
//char buf[BUFSIZE + 1]; // ������ �ۼ��� ���� -> WSABUF ����ü�� ���� ����
WSABUF send_buf;
char send_buffer[BUFSIZE];
WSABUF recv_buf;
char recv_buffer[BUFSIZE];
					  
// ���� �Լ� ���� ��� �� ����
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

DWORD WINAPI ClientMain(LPVOID arg)
{
	int retval;
	DWORD iobyte, ioflag = 0;

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
	retval = WSAConnect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr), NULL, NULL, NULL, NULL);

	// Ŭ���̾�Ʈ�� WSAAsyncSelect ���� ����ϴ°� ���ٰ� ����. �ֿ���?
	WSAAsyncSelect(sock, handle, WM_SOCKET, FD_CLOSE | FD_READ);

	// �ۼ��Ź��� ���
	send_buf.buf = send_buffer;
	send_buf.len = BUFSIZE;
	recv_buf.buf = recv_buffer;
	recv_buf.len = BUFSIZE;


	retval = WSASend(sock, &send_buf, 1, &iobyte, 0, NULL, NULL);

	retval = WSARecv(sock, &recv_buf, 1, &iobyte, &ioflag, NULL, NULL);

	return 0;
}