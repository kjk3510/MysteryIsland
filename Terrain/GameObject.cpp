#include "GameObject.h"
#include "Effects.h"


CGameObject::CGameObject() : mVB(0), mIB(0), mDiffuseMapSRV(0)
{
	ZeroMemory(&mMat, sizeof(Material));

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mWorld, I);

}


CGameObject::~CGameObject()
{
	ReleaseCOM(mVB);
	ReleaseCOM(mIB);
	ReleaseCOM(mDiffuseMapSRV);
}


void CGameObject::InitObject(ID3D11Device* device, const InitInfo& initInfo)
{
	mWorld = initInfo.Pos;

	mMat.Ambient = initInfo.Mat.Ambient;
	mMat.Diffuse = initInfo.Mat.Diffuse;
	mMat.Specular = initInfo.Mat.Specular;

	FbxLoader.LoadModel(initInfo.FbxFileName, Pos, Index);

	HR(D3DX11CreateShaderResourceViewFromFile(device,
		initInfo.FbxTextureName.c_str(), 0, 0, &mDiffuseMapSRV, 0));

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * Pos.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &Pos[0];
	HR(device->CreateBuffer(&vbd, &vinitData, &mVB));

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * Index.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &Index[0];
	HR(device->CreateBuffer(&ibd, &iinitData, &mIB));
}

void CGameObject::DrawObject(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3])
{

}

CPlayer::CPlayer()
{
	mCam.SetPosition(0.0f, 2.0f, 100.0f);
}


CPlayer::~CPlayer()
{

}

void CPlayer::InitObject(ID3D11Device* device, const InitInfo& initInfo)
{
	CGameObject::InitObject(device, initInfo);
}

void CPlayer::DrawObject(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3])
{
	dc->IASetInputLayout(InputLayouts::Fbx);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(Vertex::Basic32);
	UINT offset = 0;

	//XMMATRIX view = XMLoadFloat4x4(&mView);
	//XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX viewProj = cam.ViewProj();

	// Set per frame constants.
	Effects::FbxFX->SetDirLights(lights);
	Effects::FbxFX->SetEyePosW(cam.GetPosition());

	dc->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	dc->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);

	// Draw the box.
	XMMATRIX world = XMLoadFloat4x4(&mWorld);
	XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
	XMMATRIX worldViewProj = world*viewProj;

	//Effects::BasicFX->SetWorld(world);
	Effects::FbxFX->SetWorldInvTranspose(worldInvTranspose);
	Effects::FbxFX->SetWorldViewProj(worldViewProj);
	Effects::FbxFX->SetTexTransform(XMMatrixIdentity());
	Effects::FbxFX->SetMaterial(mMat);
	Effects::FbxFX->SetDiffuseMap(mDiffuseMapSRV);

	ID3DX11EffectTechnique* activeTech = Effects::FbxFX->Light2TexTech;

	D3DX11_TECHNIQUE_DESC techDesc;
	activeTech->GetDesc(&techDesc);

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		activeTech->GetPassByIndex(p)->Apply(0, dc);
		dc->DrawIndexed(Index.size(), 0, 0);
	}
}