#include "GameObject.h"
#include "Effects.h"
#include "GeometryGenerator.h"
#include "RenderStates.h"

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
	mWorld._41 = initInfo.Pos.x;
	mWorld._42 = initInfo.Pos.y;
	mWorld._43 = initInfo.Pos.z;

	mMat.Ambient = initInfo.Mat.Ambient;
	mMat.Diffuse = initInfo.Mat.Diffuse;
	mMat.Specular = initInfo.Mat.Specular;

	mShadowMat.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mShadowMat.Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.5f);
	mShadowMat.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 16.0f);

	HR(D3DX11CreateShaderResourceViewFromFile(device,
		initInfo.TextureName.c_str(), 0, 0, &mDiffuseMapSRV, 0));
}

void CGameObject::UpdateObject(float dt)
{

}

void CGameObject::DrawObject(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3])
{

}

void CGameObject::BuildGeometryBuffers(ID3D11Device* pd3dDevice)
{

}


CPlayer::CPlayer() : mMoveSpeed(1.0f), mAngle(0.0f)
{

}


CPlayer::~CPlayer()
{

}

void CPlayer::InitObject(ID3D11Device* pd3dDevice, const InitInfo& initInfo)
{
	CGameObject::InitObject(pd3dDevice, initInfo);

	BuildGeometryBuffers(pd3dDevice);

	mCam.SetPosition(0.0f, 2.0f, 100.0f);
}

void CPlayer::UpdateObject(float dt, float height)
{
	XMMATRIX W = XMLoadFloat4x4(&mWorld);

	if (GetAsyncKeyState('W') & 0x8000) {
		//mCam.Walk(100.0f*dt);
		XMMATRIX T = XMMatrixTranslation(0, 0, 100.0f*dt);
		XMStoreFloat4x4(&mWorld, T*W);
	}
	if (GetAsyncKeyState('S') & 0x8000) {
		//mCam.Walk(-100.0f*dt);
		XMMATRIX T = XMMatrixTranslation(0, 0, -100.0f*dt);
		XMStoreFloat4x4(&mWorld, T*W);
	}
	if (GetAsyncKeyState('A') & 0x8000) {
		//mCam.Strafe(-100.0f*dt);
		XMMATRIX T = XMMatrixTranslation(-100.0f*dt, 0, 0);
		XMStoreFloat4x4(&mWorld, T*W);
	}
	if (GetAsyncKeyState('D') & 0x8000) {
		//mCam.Strafe(100.0f*dt);
		XMMATRIX T = XMMatrixTranslation(100.0f*dt, 0, 0);
		XMStoreFloat4x4(&mWorld, T*W);
	}
	mWorld._42 = height;
	mCam.ThirdCamera(GetPosition());
	mCam.SetPosition(mCam.GetPosition().x, height + 10.0f, mCam.GetPosition().z);
	mCam.UpdateViewMatrix();
}

void CPlayer::DrawObject(ID3D11DeviceContext* pd3dImmediateContext, const Camera& cam, DirectionalLight lights[3], PointLight pointLight)
{
	UINT stride = sizeof(Vertex::Basic32);
	UINT offset = 0;

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	ID3DX11EffectTechnique* ObjectTech;
	ObjectTech = Effects::BasicFX->Light2TexTech;

	D3DX11_TECHNIQUE_DESC techDesc;
	ObjectTech->GetDesc(&techDesc);

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		ID3DX11EffectPass* pass = ObjectTech->GetPassByIndex(p);

		pd3dImmediateContext->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
		pd3dImmediateContext->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);

		// Set per object constants.
		XMMATRIX world = XMLoadFloat4x4(&mWorld);
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*mCam.ViewProj();

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
		Effects::BasicFX->SetPointLight(&pointLight);
		Effects::BasicFX->SetMaterial(mMat);
		Effects::BasicFX->SetDiffuseMap(mDiffuseMapSRV);

		pass->Apply(0, pd3dImmediateContext);
		pd3dImmediateContext->DrawIndexed(36, 0, 0);
	}

	ID3DX11EffectTechnique* shadowTech;
	shadowTech = Effects::BasicFX->Light3Tech;

	shadowTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		ID3DX11EffectPass* pass = shadowTech->GetPassByIndex(p);

		pd3dImmediateContext->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
		pd3dImmediateContext->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);

		XMVECTOR shadowPlane = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // xz plane
		XMVECTOR toMainLight = -XMLoadFloat3(&lights[0].Direction);
		XMMATRIX S = XMMatrixShadow(shadowPlane, toMainLight);
		XMMATRIX shadowOffsetY = XMMatrixTranslation(0.0f, 0.001f, 0.0f);

		XMMATRIX world = XMLoadFloat4x4(&mWorld)*S*shadowOffsetY;
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world * mCam.ViewProj();

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetMaterial(mShadowMat);

		pd3dImmediateContext->OMSetDepthStencilState(RenderStates::NoDoubleBlendDSS, 0);
		pass->Apply(0, pd3dImmediateContext);
		pd3dImmediateContext->DrawIndexed(36, 0, 0);

		// Restore default states.
		pd3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff);
		pd3dImmediateContext->OMSetDepthStencilState(0, 0);
	}
}

void CPlayer::BuildGeometryBuffers(ID3D11Device* pd3dDevice)
{
	GeometryGenerator::MeshData box;

	GeometryGenerator geoGen;
	geoGen.CreateBox(5.0f, 5.0f, 5.0f, box);

	std::vector<Vertex::Basic32> vertices(box.Vertices.size());

	for (UINT i = 0; i < box.Vertices.size(); ++i)
	{
		vertices[i].Pos = box.Vertices[i].Position;
		vertices[i].Normal = box.Vertices[i].Normal;
		vertices[i].Tex = box.Vertices[i].TexC;
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * box.Vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(pd3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * box.Indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;

	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &box.Indices[0];
	HR(pd3dDevice->CreateBuffer(&ibd, &iinitData, &mIB));
}

void CPlayer::RotateY(float angle)
{
	XMMATRIX R = XMMatrixRotationY(angle);
	XMMATRIX W = XMLoadFloat4x4(&mWorld);
	XMStoreFloat4x4(&mWorld, R*W);

	mCam.RevolvePlayer(angle, GetPosition());
	//mCam.RotateY(angle);
}

XMFLOAT3 CPlayer::GetPosition()
{
	//XMMATRIX W = XMLoadFloat4x4(&mWorld);
	//XMVECTOR P = XMVectorSet(0, 0, 0, 1);
	//XMFLOAT3 Pos;
	//XMStoreFloat3(&Pos, XMVector3TransformCoord(P, W));
	//return Pos;
	XMFLOAT3 Pos(mWorld._41, mWorld._42, mWorld._43);
	return Pos;
}

//void CPlayer::Move(XMFLOAT3 dir, float dt)
//{
//	XMVECTOR s = XMVectorReplicate(dt * 10.0f);
//	XMVECTOR l = XMLoadFloat3(&dir);
//	//XMVECTOR p = XMLoadFloat3(&mPosition);
//	//XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, l, p));
//}
//
//void CPlayer::InputKeyboardMessage(float dt)
//{
//	if (GetAsyncKeyState('W') & 0x8000)
//		Move(FRONT, dt);
//
//	if (GetAsyncKeyState('S') & 0x8000)
//		Move(BACK, dt);
//
//	if (GetAsyncKeyState('A') & 0x8000)
//		Move(LEFT, dt);
//
//	if (GetAsyncKeyState('D') & 0x8000)
//		Move(RIGHT, dt);
//}
//
//void RotateYObject(float angle)
//{
//
//}