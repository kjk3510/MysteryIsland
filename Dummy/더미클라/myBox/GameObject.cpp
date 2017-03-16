#include "GameObject.h"



CGameObject::CGameObject(ID3D11Device *pd3dDevice)
{
	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&m_World, I);

	m_pCube = new CMesh(pd3dDevice);
}


CGameObject::~CGameObject()
{
}

void CGameObject::DrawObject(ID3D11DeviceContext* pd3dImmediateContext)
{
	m_pCube->DrawMesh(pd3dImmediateContext);
}

void CGameObject::ReleaseObject()
{
	m_pCube->ReleaseBuffer();
	delete m_pCube;
}
