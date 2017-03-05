//***************************************************************************************
// BoxDemo.cpp by Frank Luna (C) 2011 All Rights Reserved.
//
// Demonstrates rendering a colored box.
//
// Controls:
//		Hold the left mouse button down and move the mouse to rotate.
//      Hold the right mouse button down to zoom in and out.
//
//***************************************************************************************

#include "d3dApp.h"
#include "d3dx11Effect.h"
#include "MathHelper.h"
#include "Shader.h"

class BoxApp : public D3DApp
{
private:
	CShader m_Shader;
	CCamera m_Camera;

public:
	BoxApp(HINSTANCE hInstance);
	~BoxApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene(); 

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	BoxApp theApp(hInstance);
	
	if( !theApp.Init() )
		return 0;
	
	return theApp.Run();
}
 

BoxApp::BoxApp(HINSTANCE hInstance)
:	D3DApp(hInstance)
{
	mMainWndCaption = L"Box Demo";
}

BoxApp::~BoxApp()
{
	m_Shader.ReleaseShader();
}

bool BoxApp::Init()
{
	if(!D3DApp::Init())
		return false;

	m_Shader.BuildObjects(md3dDevice);
	m_Shader.BuildFX(md3dDevice);
	m_Shader.BuildVertexLayout(md3dDevice);

	return true;
}

void BoxApp::OnResize()
{
	D3DApp::OnResize();

	m_Camera.GenerateProjectionMatrix(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
}

void BoxApp::UpdateScene(float dt)
{
	m_Camera.GenerateViewMatrix();
	//m_Camera.UpdateCamera(&m_Shader);
}

void BoxApp::DrawScene()
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_Shader.DrawScene(md3dImmediateContext, &m_Camera);

	HR(mSwapChain->Present(0, 0));
}

void BoxApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	m_Camera.OnMouseDown(&mhMainWnd, btnState, x, y);
}

void BoxApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	m_Camera.OnMouseUp(btnState, x, y);
}

void BoxApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	m_Camera.OnMouseMove(btnState, x, y);
}