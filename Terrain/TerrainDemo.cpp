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
#include "ShadowMap.h"
#include "ParticleSystem.h"

//enum RenderOptions
//{
//	Lighting = 0,
//	Textures = 1,
//	TexturesAndFog = 2
//};

struct BoundingSphere
{
	BoundingSphere() : Center(0.0f, 0.0f, 0.0f), Radius(0.0f) {}
	XMFLOAT3 Center;
	float Radius;
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

	void OnMouseDown(WPARAM btnState, UINT msg, int x, int y);
	void OnMouseUp(WPARAM btnState, UINT msg, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

	void BuildShadowTransform();
	void DrawSceneToShadowMap();

private:
	Sky* mSky;
	Terrain mTerrain;

	DirectionalLight mDirLights[3];
	PointLight mPointLight;

	//Camera mCam;

	bool mWalkCamMode;

	POINT mLastMousePos;

	//-------------Test---------------------
	CWater mWater;
	//RenderOptions mRenderOptions;
	CPlayer mPlayer;

	//------------Shadow--------------------
	BoundingSphere mSceneBounds;

	static const int SMapSize = 2048;
	ShadowMap* mSmap;
	XMFLOAT4X4 mLightView;
	XMFLOAT4X4 mLightProj;
	XMFLOAT4X4 mShadowTransform;
	//-------------Particle

	ID3D11ShaderResourceView* mFlareTexSRV;
	ID3D11ShaderResourceView* mRandomTexSRV;

	ParticleSystem mFire;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	TerrainApp theApp(hInstance);
	
	if( !theApp.Init() )
		return 0;
	
	return theApp.Run();
}

TerrainApp::TerrainApp(HINSTANCE hInstance)
: D3DApp(hInstance), mSky(0), mWalkCamMode(true),
  mSmap(0), mRandomTexSRV(0), mFlareTexSRV(0)
{
	mMainWndCaption = L"Mystery Island";
	mEnable4xMsaa = false;

	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	mDirLights[0].Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[0].Diffuse  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
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

	mPointLight.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mPointLight.Diffuse = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	mPointLight.Specular = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	mPointLight.Att = XMFLOAT3(1.0f, 0.0f, 0.0f);
	mPointLight.Range = 30.0f;
	mPointLight.Position.x = 0.0f;
	mPointLight.Position.y = 0.0f;
	mPointLight.Position.z = 120.0f;

	//------------Shadow--------------------
	mSceneBounds.Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mSceneBounds.Radius = sqrtf(10.0f*10.0f + 15.0f*15.0f);
}

TerrainApp::~TerrainApp()
{
	md3dImmediateContext->ClearState();
	
	SafeDelete(mSky);
	SafeDelete(mSmap);

	ReleaseCOM(mRandomTexSRV);
	ReleaseCOM(mFlareTexSRV);

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
	tii.HeightMapFilename = L"Textures/Terrain.raw";
	tii.LayerMapFilename0 = L"Textures/grass.dds";
	tii.LayerMapFilename1 = L"Textures/darkdirt.dds";
	tii.LayerMapFilename2 = L"Textures/stone.dds";
	tii.LayerMapFilename3 = L"Textures/lightdirt.dds";
	tii.LayerMapFilename4 = L"Textures/snow.dds";
	tii.BlendMapFilename = L"Textures/blend.dds";
	//tii.HeightScale = 128.0f;
	//tii.HeightmapWidth = 2049;
	//tii.HeightmapHeight = 2049;
	//tii.CellSpacing = 0.5f;
	tii.HeightScale = 50.0f;
	tii.HeightmapWidth = 2049;
	tii.HeightmapHeight = 2049;
	tii.CellSpacing = 0.5f;

	mTerrain.Init(md3dDevice, md3dImmediateContext, tii);
	std::cout << "터레인 초기화" << std::endl;
	mWater.InitWater(md3dDevice, 320, 320, 5.0f);
	std::cout << "물 초기화" << std::endl;

	//---------Test-------------
	CGameObject::InitInfo info;
	info.TextureName = L"Textures/WoodCrate01.dds";
	info.Mat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	info.Mat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	info.Mat.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);
	info.Pos = XMFLOAT3(0.0f, 0.0f, 120.0f);
	mPlayer.InitObject(md3dDevice, info);

	//------------Shadow--------------------
	mSmap = new ShadowMap(md3dDevice, SMapSize, SMapSize);

	//------------Particle
	mRandomTexSRV = d3dHelper::CreateRandomTexture1DSRV(md3dDevice);

	std::vector<std::wstring> flares;
	flares.push_back(L"Textures\\flare0.dds");
	mFlareTexSRV = d3dHelper::CreateTexture2DArraySRV(md3dDevice, md3dImmediateContext, flares);

	mFire.Init(md3dDevice, Effects::FireFX, mFlareTexSRV, mRandomTexSRV, 500);
	mFire.SetEmitPos(XMFLOAT3(0.0f, 1.0f, 120.0f));

	return true;
}

void TerrainApp::OnResize()
{
	D3DApp::OnResize();

	mPlayer.GetCamera()->SetLens(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
}

void TerrainApp::UpdateScene(float dt)
{
	XMFLOAT3 PlayerPos = mPlayer.GetPosition();
	float playerY = mTerrain.GetHeight(PlayerPos.x, PlayerPos.z) + 2.5f;
	float camY = mTerrain.GetHeight(mPlayer.GetCamera()->GetPosition().x, mPlayer.GetCamera()->GetPosition().z);
	mPlayer.UpdateObject(dt, playerY, camY);
	mWater.UpdateWater(md3dImmediateContext, dt);
	mFire.Update(dt, mTimer.TotalTime());
	BuildShadowTransform();
}

void TerrainApp::DrawScene()
{
	//mSmap->BindDsvAndSetNullRenderTarget(md3dImmediateContext);

	//DrawSceneToShadowMap();

	//md3dImmediateContext->RSSetState(0);

	//
	// Restore the back and depth buffer to the OM stage.
	//
	//ID3D11RenderTargetView* renderTargets[1] = { mRenderTargetView };
	//md3dImmediateContext->OMSetRenderTargets(1, renderTargets, mDepthStencilView);
	//md3dImmediateContext->RSSetViewports(1, &mScreenViewport);


	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Silver));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	md3dImmediateContext->IASetInputLayout(InputLayouts::Basic32);
    md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Effects::BasicFX->SetCubeMap(mSky->CubeMapSRV());
	//Effects::BasicFX->SetShadowMap(mSmap->DepthMapSRV());

	mPlayer.DrawObject(md3dImmediateContext, *mPlayer.GetCamera(), mDirLights, mPointLight, mShadowTransform);

	if( GetAsyncKeyState('1') & 0x8000 )
		md3dImmediateContext->RSSetState(RenderStates::WireframeRS);

	//mTerrain.Draw(md3dImmediateContext, *mPlayer.GetCamera(), mDirLights);
	mTerrain.Draw(md3dImmediateContext, *mPlayer.GetCamera(), mDirLights, mPointLight);

	md3dImmediateContext->RSSetState(0);

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	// Draw particle systems last so it is blended with scene.
	mFire.SetEyePos(mPlayer.GetCamera()->GetPosition());
	mFire.Draw(md3dImmediateContext, *mPlayer.GetCamera());
	md3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff); // restore default

	//mSky->Draw(md3dImmediateContext, *mPlayer.GetCamera());
	mSky->Draw(md3dImmediateContext, *mPlayer.GetCamera());

	mWater.Draw(md3dImmediateContext, *mPlayer.GetCamera(), mDirLights);

	// restore default states, as the SkyFX changes them in the effect file.
	md3dImmediateContext->RSSetState(0);
	md3dImmediateContext->OMSetDepthStencilState(0, 0);

	HR(mSwapChain->Present(0, 0));
}

void TerrainApp::OnMouseDown(WPARAM btnState, UINT msg, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;
	
	if(msg == WM_RBUTTONDOWN)
		mPlayer.GetCamera()->SetLookAround(true);

	SetCapture(mhMainWnd);
}

void TerrainApp::OnMouseUp(WPARAM btnState, UINT msg, int x, int y)
{
	mPlayer.InitCamera();

	if (msg == WM_RBUTTONUP)
		mPlayer.GetCamera()->SetLookAround(false);

	ReleaseCapture();
}

void TerrainApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	// Make each pixel correspond to a quarter of a degree.
	float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
	float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

	if( (btnState & MK_LBUTTON) != 0 )
	{
		mPlayer.RotateY(dx);
	}
	if ((btnState & MK_RBUTTON) != 0)
	{
		mPlayer.GetCamera()->Pitch(dy);
		mPlayer.GetCamera()->RotateY(dx); 
	}
	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void TerrainApp::BuildShadowTransform()
{
	// Only the first "main" light casts a shadow.
	XMVECTOR lightDir = XMLoadFloat3(&mDirLights[0].Direction);
	XMVECTOR lightPos = -2.0f*mSceneBounds.Radius*lightDir;
	XMVECTOR targetPos = XMLoadFloat3(&mSceneBounds.Center);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(lightPos, targetPos, up);

	// Transform bounding sphere to light space.
	XMFLOAT3 sphereCenterLS;
	XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, V));

	// Ortho frustum in light space encloses scene.
	float l = sphereCenterLS.x - mSceneBounds.Radius;
	float b = sphereCenterLS.y - mSceneBounds.Radius;
	float n = sphereCenterLS.z - mSceneBounds.Radius;
	float r = sphereCenterLS.x + mSceneBounds.Radius;
	float t = sphereCenterLS.y + mSceneBounds.Radius;
	float f = sphereCenterLS.z + mSceneBounds.Radius;
	XMMATRIX P = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);

	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	XMMATRIX S = V*P*T;

	XMStoreFloat4x4(&mLightView, V);
	XMStoreFloat4x4(&mLightProj, P);
	XMStoreFloat4x4(&mShadowTransform, S);
}

void TerrainApp::DrawSceneToShadowMap()
{
	XMMATRIX view = XMLoadFloat4x4(&mLightView);
	XMMATRIX proj = XMLoadFloat4x4(&mLightProj);
	XMMATRIX viewProj = XMMatrixMultiply(view, proj);

	Effects::BuildShadowMapFX->SetEyePosW(mPlayer.GetCamera()->GetPosition());
	Effects::BuildShadowMapFX->SetViewProj(viewProj);

	// These properties could be set per object if needed.
	Effects::BuildShadowMapFX->SetHeightScale(0.07f);
	Effects::BuildShadowMapFX->SetMaxTessDistance(1.0f);
	Effects::BuildShadowMapFX->SetMinTessDistance(25.0f);
	Effects::BuildShadowMapFX->SetMinTessFactor(1.0f);
	Effects::BuildShadowMapFX->SetMaxTessFactor(5.0f);

	ID3DX11EffectTechnique* tessSmapTech = Effects::BuildShadowMapFX->BuildShadowMapTech;
	ID3DX11EffectTechnique* smapTech = Effects::BuildShadowMapFX->BuildShadowMapTech;

	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	smapTech = Effects::BuildShadowMapFX->BuildShadowMapTech;
	tessSmapTech = Effects::BuildShadowMapFX->BuildShadowMapTech;


	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;

	mPlayer.ShadowDraw(md3dImmediateContext, mLightView, mLightProj);

	// FX sets tessellation stages, but it does not disable them.  So do that here
	// to turn off tessellation.
	md3dImmediateContext->HSSetShader(0, 0, 0);
	md3dImmediateContext->DSSetShader(0, 0, 0);

	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}