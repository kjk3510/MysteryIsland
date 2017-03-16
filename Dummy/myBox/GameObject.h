#pragma once
#include "Mesh.h"

class CGameObject
{
private:
	XMFLOAT4X4 m_World;
	CMesh* m_pCube;

public:
	CGameObject(ID3D11Device *pd3dDevice);
	~CGameObject();
	void DrawObject(ID3D11DeviceContext* pd3dImmediateContext);
	void ReleaseObject();
	XMFLOAT4X4 GetWorldMatrix() { return m_World; }
};

