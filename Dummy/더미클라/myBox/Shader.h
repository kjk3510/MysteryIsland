#pragma once
#include "Camera.h"
#include "GameObject.h"

class CShader
{
private:
	ID3DX11Effect* m_FX;
	ID3DX11EffectTechnique* m_Tech;
	ID3DX11EffectMatrixVariable* m_WorldViewProj;

	ID3D11InputLayout* m_InputLayout;
	//CMesh* m_pBox;
	CGameObject* m_pCube;

public:
	CShader();
	~CShader();

	void BuildObjects(ID3D11Device *pd3dDevice);
	void BuildFX(ID3D11Device *pd3dDevice);
	void BuildVertexLayout(ID3D11Device *pd3dDevice);
	void DrawScene(ID3D11DeviceContext* pd3dImmediateContext, CCamera* pCamera);
	void ReleaseShader();
	void SetWorldViewProj(CXMMATRIX M) { m_WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
};

