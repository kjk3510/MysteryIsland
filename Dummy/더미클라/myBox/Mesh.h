#pragma once
#include <d3dUtil.h>

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

class CMesh
{
private:
	ID3D11Buffer* m_VertexBuffer;
	ID3D11Buffer* m_IndexBuffer;

public:
	CMesh(ID3D11Device *pd3dDevice);
	~CMesh();
	void DrawMesh(ID3D11DeviceContext* pd3dImmediateContext);
	void ReleaseBuffer();
};

