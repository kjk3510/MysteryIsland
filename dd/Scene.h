#pragma once
#include "Shader.h"

//�������� ����
#define MAX_LIGHTS		4 
#define POINT_LIGHT		1.0f
#define SPOT_LIGHT		2.0f
#define DIRECTIONAL_LIGHT	3.0f
//�������� ����

//1���� ������ ǥ���ϴ� ����ü�̴�. 
struct LIGHT 
{
	D3DXCOLOR m_d3dxcAmbient;
	D3DXCOLOR m_d3dxcDiffuse;
	D3DXCOLOR m_d3dxcSpecular;
	D3DXVECTOR3 m_d3dxvPosition;
	float m_fRange;
	D3DXVECTOR3 m_d3dxvDirection;
	float m_nType;
	D3DXVECTOR3 m_d3dxvAttenuation;
	float m_fFalloff;
	float m_fTheta; //cos(m_fTheta)
	float m_fPhi; //cos(m_fPhi)
	float m_bEnable;
	float padding;
};

//��� ���۴� ũ�Ⱑ �ݵ�� 16 ����Ʈ�� ����� �Ǿ�� �Ѵ�. 
struct LIGHTS
{
	LIGHT m_pLights[MAX_LIGHTS];
	D3DXCOLOR m_d3dxcGlobalAmbient;
	D3DXVECTOR4 m_d3dxvCameraPosition;
};

class CScene
{
	//���� ���� �Լ��� ����
private:
	LIGHTS *m_pLights;
	ID3D11Buffer *m_pd3dcbLights;

public:
	void CreateShaderVariables(ID3D11Device *pd3dDevice);
	void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, LIGHTS *pLights);
	void ReleaseShaderVariables();
	//���� ���� �Լ��� ����

public:
	CHeightMapTerrain *GetTerrain();

	CGameObject *PickObjectPointedByCursor(int xClient, int yClient);
	void SetCamera(CCamera *pCamera) { m_pCamera = pCamera; }

private:
	CCamera *m_pCamera;
	CGameObject *m_pSelectedObject;

private:
	//���� ���̴����� ����Ʈ(�迭)�̴�.
	//CShader *m_pShaders;
	CShader **m_ppShaders;
	int m_nShaders;

public:
	CScene();
	~CScene();

	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildObjects(ID3D11Device *pd3dDevice);
	void ReleaseObjects();

	bool ProcessInput();
	void AnimateObjects(float fTimeElapsed);
	void Render(ID3D11DeviceContext*pd3dDeviceContext, CCamera *pCamera);
};