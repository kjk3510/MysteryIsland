//***************************************************************************************
// TerrainDemo.cpp by Frank Luna (C) 2011 All Rights Reserved.
//
// Demonstrates terrain rendering using tessellation and multitexturing.
//
// Controls:
//		Hold the left mouse button down and move the mouse to rotate.
//      Hold the right mouse button down to zoom in and out.
//
//***************************************************************************************

#include "d3dApp.h"
#include "d3dx11Effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include "LightHelper.h"
#include "Effects.h"
#include "Vertex.h"
#include "RenderStates.h"
#include "Camera.h"
#include "Sky.h"
#include "Terrain.h"
#include "GameObject.h"
#include "Water.h"
#include "RenderStates.h"

#pragma comment (lib, "ws2_32.lib")
#include <WinSock2.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <Windows.h>
#include "..\..\Server_iocp\Server_iocp\Protocol.h"

#define WM_SOCKET		WM_USER + 1
#define SERVERIP "127.0.0.1"

using namespace std;

HWND main_window_handle = NULL;
HINSTANCE main_instance = NULL;

DWORD      in_packet_size = 0;
int      saved_packet_size = 0;
int g_myid;

WSABUF send_buf;
char send_buffer[BUFSIZ];
WSABUF recv_buf;
char recv_buffer[BUFSIZ];
char packet_buffer[BUFSIZ];

SOCKET sock;

void ReadPacket(SOCKET sock);
void ProcessPacket(char *ptr);

void err_quit(char *msg);
void err_display(char *msg);
void clienterror();

enum RenderOptions
{
	Lighting = 0,
	Textures = 1,
	TexturesAndFog = 2
};
 
class TerrainApp : public D3DApp
{
public:
	TerrainApp(HINSTANCE hInstance);
	~TerrainApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene(); 

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	Sky* mSky;
	Terrain mTerrain;

	DirectionalLight mDirLights[3];

	//Camera mCam;

	bool mWalkCamMode;

	POINT mLastMousePos;

	//-------------Test---------------------
	CWater mWater;
	RenderOptions mRenderOptions;
	CPlayer mPlayer;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	WNDCLASS winclass;	// this will hold the class we create
	HWND	 hwnd;		// generic window handle
	MSG		 msg;		// generic message

	main_window_handle = hwnd;
	main_instance = hInstance;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// WSASocket(주소체계, 소켓타입, 프로토콜, 프로토콜정보, 몰라, 몰라) <-> socket(주소체계, 소켓타입, 프로토콜)
	sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	WSAAsyncSelect(sock, main_window_handle, WM_SOCKET, FD_CLOSE | FD_READ);

	// WSAConnect(소켓, 서버주소, 주소크기, 몰라, 몰라, 몰라, 몰라) <-> connet(소켓, 서버주소, 주소크기)
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	int retval = WSAConnect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr), NULL, NULL, NULL, NULL);

	std::cout << "connet complete" << std::endl;

	TerrainApp theApp(hInstance);
	
	if( !theApp.Init() )
		return 0;
	
	return theApp.Run();
}

TerrainApp::TerrainApp(HINSTANCE hInstance)
: D3DApp(hInstance), mSky(0), mWalkCamMode(true), mRenderOptions(RenderOptions::TexturesAndFog)
{
	mMainWndCaption = L"Mystery Island";
	mEnable4xMsaa = false;

	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	//mCam.SetPosition(0.0f, 2.0f, 100.0f);

	mDirLights[0].Ambient  = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	mDirLights[0].Diffuse  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mDirLights[0].Specular = XMFLOAT4(0.8f, 0.8f, 0.7f, 1.0f);
	mDirLights[0].Direction = XMFLOAT3(0.707f, -0.707f, 0.0f);

	mDirLights[1].Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[1].Diffuse  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[1].Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[1].Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

	mDirLights[2].Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[2].Diffuse  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[2].Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[2].Direction = XMFLOAT3(-0.57735f, -0.57735f, -0.57735f);
}

TerrainApp::~TerrainApp()
{
	md3dImmediateContext->ClearState();
	
	SafeDelete(mSky);

	Effects::DestroyAll();
	InputLayouts::DestroyAll();
	RenderStates::DestroyAll();
}

bool TerrainApp::Init()
{
	if(!D3DApp::Init())
		return false;

	// Must init Effects first since InputLayouts depend on shader signatures.
	Effects::InitAll(md3dDevice);
	InputLayouts::InitAll(md3dDevice);
	RenderStates::InitAll(md3dDevice);

	mSky = new Sky(md3dDevice, L"Textures/grasscube1024.dds", 5000.0f);
	std::cout << "스카이박스 초기화" << std::endl;
	Terrain::InitInfo tii;
	tii.HeightMapFilename = L"Textures/terrain.raw";
	tii.LayerMapFilename0 = L"Textures/grass.dds";
	tii.LayerMapFilename1 = L"Textures/darkdirt.dds";
	tii.LayerMapFilename2 = L"Textures/stone.dds";
	tii.LayerMapFilename3 = L"Textures/lightdirt.dds";
	tii.LayerMapFilename4 = L"Textures/snow.dds";
	tii.BlendMapFilename = L"Textures/blend.dds";
	tii.HeightScale = 50.0f;
	tii.HeightmapWidth = 2049;
	tii.HeightmapHeight = 2049;
	//tii.HeightScale = 128.0f;
	//tii.HeightmapWidth = 1024;
	//tii.HeightmapHeight = 1024;
	tii.CellSpacing = 0.5f;

	mTerrain.Init(md3dDevice, md3dImmediateContext, tii);
	std::cout << "터레인 초기화" << std::endl;
	mWater.InitWater(md3dDevice, 320, 320, 5.0f);
	std::cout << "물 초기화" << std::endl;

	//---------Test-------------
	CGameObject::InitInfo info;
	info.TextureName = L"crawlerTexture.png";
	info.Mat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	info.Mat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	info.Mat.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);
	info.Pos = XMFLOAT3(0.0f, 0.0f, 150.0f);
	mPlayer.InitObject(md3dDevice, info);

	return true;
}

void TerrainApp::OnResize()
{
	D3DApp::OnResize();

	mPlayer.GetCamera()->SetLens(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
}

void TerrainApp::UpdateScene(float dt)
{
	if (GetAsyncKeyState('R') & 0x8000)
		mDirLights[0].Ambient.x += 0.1;
	if (GetAsyncKeyState('F') & 0x8000)
		mDirLights[0].Ambient.x -= 0.1;
	if (GetAsyncKeyState('T') & 0x8000)
		mDirLights[0].Diffuse.x += 0.1;
	if (GetAsyncKeyState('G') & 0x8000)
		mDirLights[0].Diffuse.x -= 0.1;
	if (GetAsyncKeyState('Y') & 0x8000)
		mDirLights[0].Specular.x += 0.1;
	if (GetAsyncKeyState('H') & 0x8000)
		mDirLights[0].Specular.x -= 0.1;

	//if (GetAsyncKeyState('U') & 0x8000)
	//	mDirLights[1].Ambient.x += 0.1;
	//if (GetAsyncKeyState('J') & 0x8000)
	//	mDirLights[1].Ambient.x -= 0.1;
	//if (GetAsyncKeyState('I') & 0x8000)
	//	mDirLights[1].Diffuse.x += 0.1;
	//if (GetAsyncKeyState('K') & 0x8000)
	//	mDirLights[1].Diffuse.x -= 0.1;
	//if (GetAsyncKeyState('O') & 0x8000)
	//	mDirLights[1].Specular.x += 0.1;
	//if (GetAsyncKeyState('L') & 0x8000)
	//	mDirLights[1].Specular.x -= 0.1;

	//if (GetAsyncKeyState('Z') & 0x8000)
	//	mDirLights[2].Ambient.x += 0.1;
	//if (GetAsyncKeyState('X') & 0x8000)
	//	mDirLights[2].Ambient.x -= 0.1;
	//if (GetAsyncKeyState('C') & 0x8000)
	//	mDirLights[2].Diffuse.x += 0.1;
	//if (GetAsyncKeyState('V') & 0x8000)
	//	mDirLights[2].Diffuse.x -= 0.1;
	//if (GetAsyncKeyState('B') & 0x8000)
	//	mDirLights[2].Specular.x += 0.1;
	//if (GetAsyncKeyState('N') & 0x8000)
	//	mDirLights[2].Specular.x -= 0.1;

	//
	// Control the camera.
	//
	//if( GetAsyncKeyState('W') & 0x8000 )
	//	mPlayer.GetCamera()->Walk(10.0f*dt);

	//if( GetAsyncKeyState('S') & 0x8000 )
	//	mPlayer.GetCamera()->Walk(-10.0f*dt);

	//if( GetAsyncKeyState('A') & 0x8000 )
	//	mPlayer.GetCamera()->Strafe(-10.0f*dt);

	//if( GetAsyncKeyState('D') & 0x8000 )
	//	mPlayer.GetCamera()->Strafe(10.0f*dt);
	/*if (GetAsyncKeyState('W') & 0x8000)
		mCam.Walk(100.0f*dt);

	if (GetAsyncKeyState('S') & 0x8000)
		mCam.Walk(-100.0f*dt);

	if (GetAsyncKeyState('A') & 0x8000)
		mCam.Strafe(-100.0f*dt);

	if (GetAsyncKeyState('D') & 0x8000)
		mCam.Strafe(100.0f*dt);*/
	//cout << mCam.GetPosition().x << " " << mCam.GetPosition().y << " " << mCam.GetPosition().z << endl;
	//
	// Walk/fly mode
	//
	if( GetAsyncKeyState('2') & 0x8000 )
		mWalkCamMode = true;
	if( GetAsyncKeyState('3') & 0x8000 )
		mWalkCamMode = false;

	// 
	// Clamp camera to terrain surface in walk mode.
	//
	//if( mWalkCamMode )
	//{
	//	//XMFLOAT3 camPos = mPlayer.GetCamera()->GetPosition();
	//	XMFLOAT3 camPos = mPlayer.GetCamera()->GetPosition();
	//	float y = mTerrain.GetHeight(camPos.x, camPos.z) + 10.0f;
	//	//mPlayer.GetCamera()->SetPosition(camPos.x, y + 2.0f, camPos.z);
	//	mPlayer.GetCamera()->SetPosition(camPos.x, y + 2.0f, camPos.z);
	//}
	//mPlayer.InputKeyboardMessage(dt);
	XMFLOAT3 PlayerPos = mPlayer.GetPosition();
	float y = mTerrain.GetHeight(PlayerPos.x, PlayerPos.z);
	mPlayer.UpdateObject(md3dImmediateContext, dt, y);
	//mPlayer.GetCamera()->UpdateViewMatrix();
	mWater.UpdateWater(md3dImmediateContext, dt);
}

void TerrainApp::DrawScene()
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Silver));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	md3dImmediateContext->IASetInputLayout(InputLayouts::Basic32);
    md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	mPlayer.DrawObject(md3dImmediateContext, *mPlayer.GetCamera(), mDirLights);

	if( GetAsyncKeyState('1') & 0x8000 )
		md3dImmediateContext->RSSetState(RenderStates::WireframeRS);

	//mTerrain.Draw(md3dImmediateContext, *mPlayer.GetCamera(), mDirLights);
	mTerrain.Draw(md3dImmediateContext, *mPlayer.GetCamera(), mDirLights);

	md3dImmediateContext->RSSetState(0);

	//mSky->Draw(md3dImmediateContext, *mPlayer.GetCamera());
	mSky->Draw(md3dImmediateContext, *mPlayer.GetCamera());

	mWater.Draw(md3dImmediateContext, *mPlayer.GetCamera(), mDirLights);

	// restore default states, as the SkyFX changes them in the effect file.
	md3dImmediateContext->RSSetState(0);
	md3dImmediateContext->OMSetDepthStencilState(0, 0);

	HR(mSwapChain->Present(0, 0));
}

void TerrainApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void TerrainApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void TerrainApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	if( (btnState & MK_LBUTTON) != 0 )
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		//mPlayer.GetCamera()->Pitch(dy);
		//mPlayer.GetCamera()->RotateY(dx);
		//mPlayer.SetRotateAngle(dx);
		//mPlayer.GetCamera()->Pitch(dy);
		//mPlayer.GetCamera()->RotateY(dx);
		mPlayer.RotateY(dx);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
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