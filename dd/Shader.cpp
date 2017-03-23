#include "stdafx.h"
#include "Shader.h"

//���� ��ȯ ����� ���� ��� ���۴� ���̴� ��ü�� ����(static) ������ ����̴�.
ID3D11Buffer *CShader::m_pd3dcbWorldMatrix = NULL;
ID3D11Buffer *CIlluminatedShader::m_pd3dcbMaterial = NULL;


CShader::CShader()
{
	m_ppObjects = NULL;
	m_nObjects = 0;

	m_pd3dVertexShader = NULL;
	m_pd3dVertexLayout = NULL;
	m_pd3dPixelShader = NULL;
}

CShader::~CShader()
{
	if (m_pd3dVertexShader) m_pd3dVertexShader->Release();
	if (m_pd3dVertexLayout) m_pd3dVertexLayout->Release();
	if (m_pd3dPixelShader) m_pd3dPixelShader->Release();
}

void CShader::BuildObjects(ID3D11Device *pd3dDevice)
{
}

void CShader::ReleaseObjects()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j]) delete m_ppObjects[j];
		delete[] m_ppObjects;
	}
}

void CShader::AnimateObjects(float fTimeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(fTimeElapsed);
	}
}

void CShader::OnPrepareRender(ID3D11DeviceContext *pd3dDeviceContext)
{
	pd3dDeviceContext->IASetInputLayout(m_pd3dVertexLayout);
	pd3dDeviceContext->VSSetShader(m_pd3dVertexShader, NULL, 0);
	pd3dDeviceContext->PSSetShader(m_pd3dPixelShader, NULL, 0);
}

void CShader::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	//���� ��ȯ ����� ���� ��� ���۸� �����Ѵ�.
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VS_CB_WORLD_MATRIX);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&bd, NULL, &m_pd3dcbWorldMatrix);
}

void CShader::ReleaseShaderVariables()
{
	//���� ��ȯ ����� ���� ��� ���۸� ��ȯ�Ѵ�.
	if (m_pd3dcbWorldMatrix) m_pd3dcbWorldMatrix->Release();
}

void CShader::UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, D3DXMATRIX *pd3dxmtxWorld)
{
	//���� ��ȯ ����� ��� ���ۿ� �����Ѵ�.
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbWorldMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);

	VS_CB_WORLD_MATRIX *pcbWorldMatrix = (VS_CB_WORLD_MATRIX *)d3dMappedResource.pData;
	D3DXMatrixTranspose(&pcbWorldMatrix->m_d3dxmtxWorld, pd3dxmtxWorld);
	pd3dDeviceContext->Unmap(m_pd3dcbWorldMatrix, 0);

	//��� ���۸� ����̽��� ����(VS_SLOT_WORLD_MATRIX)�� �����Ѵ�.
	pd3dDeviceContext->VSSetConstantBuffers(VS_SLOT_WORLD_MATRIX, 1, &m_pd3dcbWorldMatrix);//���� ��Ʈ������ 1
}


void CShader::CreateVertexShaderFromFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11VertexShader **ppd3dVertexShader, D3D11_INPUT_ELEMENT_DESC *pd3dInputLayout, UINT nElements, ID3D11InputLayout **ppd3dVertexLayout)
{
	HRESULT hResult;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob *pd3dShaderBlob = NULL, *pd3dErrorBlob = NULL;
	/*����(pszFileName)���� ���̴� �Լ�(pszShaderName)�� �������Ͽ� �����ϵ� ���̴� �ڵ��� �޸� �ּ�(pd3dShaderBlob)�� ��ȯ�Ѵ�.*/
	if (SUCCEEDED(hResult = D3DX11CompileFromFile(pszFileName, NULL, NULL, pszShaderName, pszShaderModel, dwShaderFlags, 0, NULL, &pd3dShaderBlob, &pd3dErrorBlob, NULL)))
	{
		//�����ϵ� ���̴� �ڵ��� �޸� �ּҿ��� ����-���̴��� �����Ѵ�. 
		pd3dDevice->CreateVertexShader(pd3dShaderBlob->GetBufferPointer(), pd3dShaderBlob->GetBufferSize(), NULL, ppd3dVertexShader);
		//�����ϵ� ���̴� �ڵ��� �޸� �ּҿ� �Է� ���̾ƿ����� ���� ���̾ƿ��� �����Ѵ�. 
		pd3dDevice->CreateInputLayout(pd3dInputLayout, nElements, pd3dShaderBlob->GetBufferPointer(), pd3dShaderBlob->GetBufferSize(), ppd3dVertexLayout);
		pd3dShaderBlob->Release();
	}
}

//�Լ��� ���̴� ���Ͽ��� �ȼ� - ���̴��� �����Ѵ�.
void CShader::CreatePixelShaderFromFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11PixelShader **ppd3dPixelShader)
{
	HRESULT hResult;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob *pd3dShaderBlob = NULL, *pd3dErrorBlob = NULL;
	/*����(pszFileName)���� ���̴� �Լ�(pszShaderName)�� �������Ͽ� �����ϵ� ���̴� �ڵ��� �޸� �ּ�(pd3dShaderBlob)�� ��ȯ�Ѵ�.*/
	if (SUCCEEDED(hResult = D3DX11CompileFromFile(pszFileName, NULL, NULL, pszShaderName, pszShaderModel, dwShaderFlags, 0, NULL, &pd3dShaderBlob, &pd3dErrorBlob, NULL)))
	{
		//�����ϵ� ���̴� �ڵ��� �޸� �ּҿ��� �ȼ�-���̴��� �����Ѵ�. 
		pd3dDevice->CreatePixelShader(pd3dShaderBlob->GetBufferPointer(), pd3dShaderBlob->GetBufferSize(), NULL, ppd3dPixelShader);
		pd3dShaderBlob->Release();
	}
}

void CShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputLayout);
	CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "VS", "vs_5_0", &m_pd3dVertexShader, d3dInputLayout, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PS", "ps_5_0", &m_pd3dPixelShader);
}

//���̴� ������ �Լ��� �����Ѵ�.
void CShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	OnPrepareRender(pd3dDeviceContext);

	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
		{
			if (m_ppObjects[j]->IsVisible(pCamera))
			{
				m_ppObjects[j]->Render(pd3dDeviceContext, pCamera);
			}
		}
	}
}


CGameObject *CShader::PickObjectByRayIntersection(D3DXVECTOR3 *pd3dxvPickPosition, D3DXMATRIX *pd3dxmtxView, MESHINTERSECTINFO *pd3dxIntersectInfo)
{
	int nIntersected = 0;
	float fNearHitDistance = FLT_MAX;
	CGameObject *pSelectedObject = NULL;
	MESHINTERSECTINFO d3dxIntersectInfo;
	/*���̴� ��ü�� ���ԵǴ� ��� ��ü�鿡 ���Ͽ� ��ŷ ������ �����ϰ� ��ü�� �ٿ�� �ڽ����� ������ �˻��Ѵ�. �����ϴ� ��ü�� �߿� ī�޶�� ���� ����� ��ü�� ������ ��ü�� ��ȯ�Ѵ�.*/
	for (int i = 0; i < m_nObjects; i++)
	{
		nIntersected = m_ppObjects[i]->PickObjectByRayIntersection(pd3dxvPickPosition, pd3dxmtxView, &d3dxIntersectInfo);
		if ((nIntersected > 0) && (d3dxIntersectInfo.m_fDistance < fNearHitDistance))
		{
			fNearHitDistance = d3dxIntersectInfo.m_fDistance;
			pSelectedObject = m_ppObjects[i];
			if (pd3dxIntersectInfo) *pd3dxIntersectInfo = d3dxIntersectInfo;
		}
	}
	return(pSelectedObject);
}

//���� ������ �޴� ���̴�
CIlluminatedShader::CIlluminatedShader()
{
}

CIlluminatedShader::~CIlluminatedShader()
{
}
void CIlluminatedShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputElements);
	CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "VSLightingColor", "vs_5_0", &m_pd3dVertexShader, d3dInputElements, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PSLightingColor", "ps_5_0", &m_pd3dPixelShader);
}
void CIlluminatedShader::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	d3dBufferDesc.ByteWidth = sizeof(MATERIAL);
	pd3dDevice->CreateBuffer(&d3dBufferDesc, NULL, &m_pd3dcbMaterial);
}

void CIlluminatedShader::ReleaseShaderVariables()
{
	if (m_pd3dcbMaterial) m_pd3dcbMaterial->Release();
}

void CIlluminatedShader::UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, MATERIAL *pMaterial)
{
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbMaterial, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	MATERIAL *pcbMaterial = (MATERIAL *)d3dMappedResource.pData;
	memcpy(pcbMaterial, pMaterial, sizeof(MATERIAL));
	pd3dDeviceContext->Unmap(m_pd3dcbMaterial, 0);
	pd3dDeviceContext->PSSetConstantBuffers(PS_SLOT_MATERIAL, 1, &m_pd3dcbMaterial);
}
//���� ������ �޴� ���̴�

//���� ������ �ȹ޴� ���̴�
CDiffusedShader::CDiffusedShader()
{
}

CDiffusedShader::~CDiffusedShader()
{
}

void CDiffusedShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputElements);
	CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "VSDiffusedColor", "vs_5_0", &m_pd3dVertexShader, d3dInputElements, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PSDiffusedColor", "ps_5_0", &m_pd3dPixelShader);
}
//���� ������ �ȹ޴� ���̴�

CSceneShader::CSceneShader()
{
}

CSceneShader::~CSceneShader()
{
}
void CSceneShader::CreateShader(ID3D11Device *pd3dDevice)
{
	CShader::CreateShader(pd3dDevice);
}

void CSceneShader::BuildObjects(ID3D11Device *pd3dDevice)
{
	int xObjects = 10, yObjects = 10, zObjects = 10, i = 0;
	/*������ü ��ü�� x-��, y-��, z-������ ���� 13���� �� 2,197��(13x13x13) �����ϰ� �� ��ü�� 16�� �����Ѵ�.*/  
		m_nObjects = (xObjects * 2 + 1)*(yObjects * 2 + 1)*(zObjects * 2 + 1) + (8 * 2);
	m_ppObjects = new CGameObject*[m_nObjects];

	//������ü �޽��� �����Ѵ�.
	CCubeMeshIlluminated *pCubeMesh = new CCubeMeshIlluminated(pd3dDevice, 12.0f, 12.0f, 12.0f);

	float fxPitch = 12.0f * 1.7f;
	float fyPitch = 12.0f * 1.7f;
	float fzPitch = 12.0f * 1.7f;
	CRotatingObject *pRotatingObject = NULL;
	for (int x = -xObjects; x <= xObjects; x++)
	{
		for (int y = -yObjects; y <= yObjects; y++)
		{
			for (int z = -zObjects; z <= zObjects; z++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(pCubeMesh);
				pRotatingObject->SetPosition(fxPitch*x, fyPitch*y, fzPitch*z);
				pRotatingObject->SetRotationAxis(D3DXVECTOR3(0.0f, 1.0f, 0.0f));
				pRotatingObject->SetRotationSpeed(10.0f*(i % 10));
				m_ppObjects[i++] = pRotatingObject;
			}
		}
	}

	//�� �޽��� �����Ѵ�.
	CSphereMeshIlluminated *pSphereMesh = new CSphereMeshIlluminated(pd3dDevice, 12.0f, 20, 20);

	/*�� ��ü�� �����ϴ� ��ü�̴�. 8���� ������ǥ���� xy-��鿡�� ������ �߽����� 45�� �������� ��ġ�Ѵ�. z-������ ȸ���Ѵ�. 8���� ������ǥ���� yz-��鿡�� ������ �߽����� 45�� �������� ��ġ�Ѵ�. x-������ ȸ���Ѵ�.*/
	CRevolvingObject *pRevolvingObject = NULL;
	float fRadius = (xObjects * fxPitch * 2.0f) * 1.5f;
	for (int j = 0; j < 8; j++)
	{
		pRevolvingObject = new CRevolvingObject();
		pRevolvingObject->SetMesh(pSphereMesh);
		pRevolvingObject->SetPosition(fRadius*cosf(D3DXToRadian(45.0f)*j), fRadius*sinf(D3DXToRadian(45.0f)*j), 0.0f);
		pRevolvingObject->SetRevolutionAxis(D3DXVECTOR3(0.0f, 0.0f, 1.0f));
		pRevolvingObject->SetRevolutionSpeed(10.0f);
		m_ppObjects[i++] = pRevolvingObject;
	}
	for (int j = 0; j < 8; j++)
	{
		pRevolvingObject = new CRevolvingObject();
		pRevolvingObject->SetMesh(pSphereMesh);
		pRevolvingObject->SetPosition(0.0f, fRadius*cosf(D3DXToRadian(45.0f)*j), fRadius*sinf(D3DXToRadian(45.0f)*j));
		pRevolvingObject->SetRevolutionAxis(D3DXVECTOR3(1.0f, 0.0f, 0.0f));
		pRevolvingObject->SetRevolutionSpeed(20.0f);
		m_ppObjects[i++] = pRevolvingObject;
	}
}

CPlayerShader::CPlayerShader()
{
}

CPlayerShader::~CPlayerShader()
{
}
void CPlayerShader::CreateShader(ID3D11Device *pd3dDevice)
{
	CShader::CreateShader(pd3dDevice);
}
void CPlayerShader::BuildObjects(ID3D11Device *pd3dDevice)
{
	m_nObjects = 1;
	m_ppObjects = new CGameObject*[m_nObjects];

	CCubeMeshDiffused *pCubeMesh = new CCubeMeshDiffused(pd3dDevice, 4.0f, 12.0f, 4.0f, D3DXCOLOR(0.5f, 0.0f, 0.0f, 0.0f));
	CTerrainPlayer *pTerrainPlayer = new CTerrainPlayer(1);
	pTerrainPlayer->SetMesh(pCubeMesh);
	pTerrainPlayer->CreateShaderVariables(pd3dDevice);
	pTerrainPlayer->ChangeCamera(pd3dDevice, THIRD_PERSON_CAMERA, 0.0f);

	m_ppObjects[0] = pTerrainPlayer;

	m_pCollisionMap = new CHeightMap(_T("CollisionMap.raw"), 780, 780, D3DXVECTOR3(1.0f, 1.0f, 1.0f));
	m_ppObjects[0]->SetCollisionMap(m_pCollisionMap);
}

void CPlayerShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	//3��Ī ī�޶��� �� �÷��̾ �������Ѵ�.
	DWORD nCameraMode = (pCamera) ? pCamera->GetMode() : 0x00;
	if (nCameraMode == THIRD_PERSON_CAMERA)
	{
		CShader::Render(pd3dDeviceContext, pCamera);
	}
}
CInstancingShader::CInstancingShader()
{

	m_pMaterial = NULL;
	for (int i = 0; i < 2; i++)
		m_pd3dCubeInstanceBuffer[i] = NULL;
}

CInstancingShader::~CInstancingShader()
{
	if (m_pMaterial) m_pMaterial->Release();
	for (int i = 0; i < 2; i++)
		if (m_pd3dCubeInstanceBuffer[i]) m_pd3dCubeInstanceBuffer[i]->Release();
}

void CInstancingShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "INSTANCEPOS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEPOS", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEPOS", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEPOS", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};
	UINT nElements = ARRAYSIZE(d3dInputLayout);
	CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "VSInstancedLightingColor", "vs_5_0", &m_pd3dVertexShader, d3dInputLayout, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PSInstancedLightingColor", "ps_5_0", &m_pd3dPixelShader);
}

ID3D11Buffer *CInstancingShader::CreateInstanceBuffer(ID3D11Device *pd3dDevice, int nObjects, UINT nBufferStride, void *pBufferData)
{
	ID3D11Buffer *pd3dInstanceBuffer = NULL;
	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	/*������ �ʱ�ȭ �����Ͱ� ������ ���� ���۷� �����Ѵ�. ��, ���߿� ������ �Ͽ� ������ ä��ų� �����Ѵ�.*/
	d3dBufferDesc.Usage = (pBufferData) ? D3D11_USAGE_DEFAULT : D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = nBufferStride * nObjects;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = (pBufferData) ? 0 : D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pBufferData;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, (pBufferData) ? &d3dBufferData : NULL, &pd3dInstanceBuffer);
	return(pd3dInstanceBuffer);
}

void CInstancingShader::BuildObjects(ID3D11Device *pd3dDevice, CHeightMapTerrain *pHeightMapTerrain, CMaterial *pMaterial, int k)
{
	m_pMaterial = pMaterial;
	if (pMaterial) pMaterial->AddRef();

	m_nInstanceBufferStride = sizeof(VS_VB_INSTANCE);
	m_nInstanceBufferOffset = 0;

	CCubeMeshIlluminated *pCubeMesh_Type_A = new CCubeMeshIlluminated(pd3dDevice, 200.0f, 24.0f, 20.0f);
	CCubeMeshIlluminated *pCubeMesh_Type_B = new CCubeMeshIlluminated(pd3dDevice, 20.0f, 24.0f, 200.0f);
	CGameObject *pGameObject = NULL;

	m_nObjects = 21;
	m_ppObjects = new CGameObject*[m_nObjects];

	/*AŸ�� �̷� ����*/
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			pGameObject = new CGameObject(1);
			pGameObject->SetMesh(pCubeMesh_Type_A);
			pGameObject->SetPosition(190.0f + (400.0f * i), 12, 100.0f + (200.0f * j));
			m_ppObjects[(i * 4) + j] = pGameObject;
		}
	}
	pGameObject = new CGameObject(1);
	pGameObject->SetMesh(pCubeMesh_Type_A);
	pGameObject->SetPosition(520.0f, 12, 400.0f);
	m_ppObjects[8] = pGameObject;	
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			pGameObject = new CGameObject(1);
			pGameObject->SetMesh(pCubeMesh_Type_A);
			pGameObject->SetPosition(190 + (260.0f * j), 12, 200.0f + (400 * i));
			m_ppObjects[9 + (i * 2) + j] = pGameObject;
		}
	}


	/*BŸ�� �̷� ����*/
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			pGameObject = new CGameObject(1);
			pGameObject->SetMesh(pCubeMesh_Type_B);
			pGameObject->SetPosition(100.0f + (580.0f * i), 12, 190.0f + (400.0f * j));
			m_ppObjects[13 + (i * 2) + j] = pGameObject;
		}
	}
	for (int i = 0; i < 2; i++)
	{
		pGameObject = new CGameObject(1);
		pGameObject->SetMesh(pCubeMesh_Type_B);
		pGameObject->SetPosition(390.0f, 12, 200.0f + (400.0f * i));
		m_ppObjects[17 + i] = pGameObject;
	}
	pGameObject = new CGameObject(1);
	pGameObject->SetMesh(pCubeMesh_Type_B);
	pGameObject->SetPosition(500.0f, 12, 400.0f);
	m_ppObjects[19] = pGameObject;

	pGameObject = new CGameObject(1);
	pGameObject->SetMesh(pCubeMesh_Type_B);
	pGameObject->SetPosition(190.0f, 12, 400.0f);
	m_ppObjects[20] = pGameObject;

	m_pd3dCubeInstanceBuffer[0] = CreateInstanceBuffer(pd3dDevice, 13, m_nInstanceBufferStride, NULL);
	pCubeMesh_Type_A->AssembleToVertexBuffer(1, &m_pd3dCubeInstanceBuffer[0], &m_nInstanceBufferStride, &m_nInstanceBufferOffset);

	m_pd3dCubeInstanceBuffer[1] = CreateInstanceBuffer(pd3dDevice, 8, m_nInstanceBufferStride, NULL);
	pCubeMesh_Type_B->AssembleToVertexBuffer(1, &m_pd3dCubeInstanceBuffer[1], &m_nInstanceBufferStride, &m_nInstanceBufferOffset);
}

//�ν��Ͻ��� ���� ������ ���ְ� �ִ�. ������ ���ٸ� ���� ����..
void CInstancingShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	OnPrepareRender(pd3dDeviceContext);
	//������ �ִٸ� ���� ������Ʈ ���ְ� ����
	if (m_pMaterial) CIlluminatedShader::UpdateShaderVariable(pd3dDeviceContext, &m_pMaterial->m_Material);

	int nCubeInstances = 0;
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;

	/* AŸ�� �̷� ������ ���� ������ */
	pd3dDeviceContext->Map(m_pd3dCubeInstanceBuffer[0], 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_VB_INSTANCE *pCubeInstances = (VS_VB_INSTANCE *)d3dMappedResource.pData;
	for (int j = 0; j < 13; j++)
	{
		if (m_ppObjects[j])
		{
			if (m_ppObjects[j]->IsVisible(pCamera))
			{
				D3DXMatrixTranspose(&pCubeInstances[nCubeInstances].m_d3dxTransform, &m_ppObjects[j]->m_d3dxmtxWorld);
				pCubeInstances[nCubeInstances++].m_d3dxColor = RANDOM_COLOR;
			}
		}
	}
	pd3dDeviceContext->Unmap(m_pd3dCubeInstanceBuffer[0], 0);
	CMesh *pCubeMesh = m_ppObjects[0]->GetMesh();
	pCubeMesh->RenderInstanced(pd3dDeviceContext, nCubeInstances, 0);


	/* BŸ�� �̷� ������ ���� ������ */
	nCubeInstances = 0;
	pd3dDeviceContext->Map(m_pd3dCubeInstanceBuffer[1], 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	pCubeInstances = (VS_VB_INSTANCE *)d3dMappedResource.pData;
	for (int j = 13; j < 21; j++)
	{
		if (m_ppObjects[j])
		{
			if (m_ppObjects[j]->IsVisible(pCamera))
			{
				D3DXMatrixTranspose(&pCubeInstances[nCubeInstances].m_d3dxTransform, &m_ppObjects[j]->m_d3dxmtxWorld);
				pCubeInstances[nCubeInstances++].m_d3dxColor = RANDOM_COLOR;
			}
		}
	}
	pd3dDeviceContext->Unmap(m_pd3dCubeInstanceBuffer[1], 0);
	pCubeMesh = m_ppObjects[13]->GetMesh();
	pCubeMesh->RenderInstanced(pd3dDeviceContext, nCubeInstances, 0);
}

///////////////////////////////////////
CTerrainShader::CTerrainShader()
{
}

CTerrainShader::~CTerrainShader()
{
}

void CTerrainShader::BuildObjects(ID3D11Device *pd3dDevice)
{
	m_nObjects = 6;
	m_ppObjects = new CGameObject*[m_nObjects];

	D3DXVECTOR3 d3dxvScale(12.0f, 0.0f, 12.0f);
	D3DXCOLOR d3dxColor(0.0f, 0.2f, 0.0f, 0.0f);
	m_ppObjects[0] = new CHeightMapTerrain(pd3dDevice, _T("../Assets/Image/Terrain/HeightMap.raw"), 65, 65, 9, 9, d3dxvScale, d3dxColor);
		
	d3dxvScale = D3DXVECTOR3(0.1f, 1.5f, 12.0f);
	d3dxColor = D3DXCOLOR(0.5f, 0.2f, 0.0f, 0.0f);
	m_ppObjects[1] = new CHeightMapTerrain(pd3dDevice, _T("HeightMap.raw"), 65, 65, 9, 9, d3dxvScale, d3dxColor, 1);
	m_ppObjects[1]->SetPosition(-13.0f, 0.0f, 0.0f);

	d3dxvScale = D3DXVECTOR3(12.0f, 1.5f, 0.1f);
	d3dxColor = D3DXCOLOR(0.5f, 0.2f, 0.0f, 0.0f);
	m_ppObjects[2] = new CHeightMapTerrain(pd3dDevice, _T("HeightMap.raw"), 65, 65, 9, 9, d3dxvScale, d3dxColor, 2);
	m_ppObjects[2]->SetPosition(0.0f, 0.0f, -13.0f);

	CGameObject* pNewObjects = new CHeightMapTerrain(pd3dDevice, _T("HeightMap.raw"), 65, 65, 9, 9, d3dxvScale, d3dxColor, 2, TRUE);
	pNewObjects->SetPosition(0.0f, 0.0f, (257.0f * 3) - 27.0f);
	m_ppObjects[3] = pNewObjects;

	d3dxvScale = D3DXVECTOR3(0.1f, 1.5f, 12.0f);
	d3dxColor = D3DXCOLOR(0.5f, 0.2f, 0.0f, 0.0f);
	pNewObjects = new CHeightMapTerrain(pd3dDevice, _T("HeightMap.raw"), 65, 65, 9, 9, d3dxvScale, d3dxColor, 1, TRUE);
	pNewObjects->SetPosition((257.0f * 3) - 27.0f, 0.0f, 0.0f);
	m_ppObjects[4] = pNewObjects;

	d3dxvScale = D3DXVECTOR3(3.0f, 0.0f, 3.0f);
	d3dxColor = D3DXCOLOR(0.0f, 0.2f, 0.0f, 0.0f);
	m_ppObjects[5] = new CHeightMapTerrain(pd3dDevice, _T("../Assets/Image/Terrain/HeightMap.raw"), 257, 257, 17, 17, d3dxvScale, d3dxColor, 0, TRUE);
	m_ppObjects[5]->SetPosition(0.0f, 95.0f, 0.0f);
}

CHeightMapTerrain *CTerrainShader::GetTerrain()
{
	return((CHeightMapTerrain *)m_ppObjects[0]);
}