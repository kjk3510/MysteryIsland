#include "stdafx.h"
#include "Mesh.h"
#include "Object.h"

void AABB::Union(D3DXVECTOR3& d3dxvMinimum, D3DXVECTOR3& d3dxvMaximum)
{
	if (d3dxvMinimum.x < m_d3dxvMinimum.x) m_d3dxvMinimum.x = d3dxvMinimum.x;
	if (d3dxvMinimum.y < m_d3dxvMinimum.y) m_d3dxvMinimum.y = d3dxvMinimum.y;
	if (d3dxvMinimum.z < m_d3dxvMinimum.z) m_d3dxvMinimum.z = d3dxvMinimum.z;
	if (d3dxvMaximum.x > m_d3dxvMaximum.x) m_d3dxvMaximum.x = d3dxvMaximum.x;
	if (d3dxvMaximum.y > m_d3dxvMaximum.y) m_d3dxvMaximum.y = d3dxvMaximum.y;
	if (d3dxvMaximum.z > m_d3dxvMaximum.z) m_d3dxvMaximum.z = d3dxvMaximum.z;
}
void AABB::Union(AABB *pAABB)
{
	Union(pAABB->m_d3dxvMinimum, pAABB->m_d3dxvMaximum);
}
void AABB::Update(D3DXMATRIX *pmtxTransform)
{
	/*�ٿ�� �ڽ��� �ּ����� �ִ����� ȸ���� �ϸ� �� �̻� �ּ����� �ִ����� ���� �ʴ´�. �׷��Ƿ� �ٿ�� �ڽ��� �ּ����� �ִ������� 8���� ������ ���ϰ� ��ȯ(ȸ��)�� �� ���� �ּ����� �ִ����� �ٽ� ����Ѵ�.*/

	D3DXVECTOR3 vVertices[8];
	vVertices[0] = D3DXVECTOR3(m_d3dxvMinimum.x, m_d3dxvMinimum.y, m_d3dxvMinimum.z);
	vVertices[1] = D3DXVECTOR3(m_d3dxvMinimum.x, m_d3dxvMinimum.y, m_d3dxvMaximum.z);
	vVertices[2] = D3DXVECTOR3(m_d3dxvMaximum.x, m_d3dxvMinimum.y, m_d3dxvMaximum.z);
	vVertices[3] = D3DXVECTOR3(m_d3dxvMaximum.x, m_d3dxvMinimum.y, m_d3dxvMinimum.z);
	vVertices[4] = D3DXVECTOR3(m_d3dxvMinimum.x, m_d3dxvMaximum.y, m_d3dxvMinimum.z);
	vVertices[5] = D3DXVECTOR3(m_d3dxvMinimum.x, m_d3dxvMaximum.y, m_d3dxvMaximum.z);
	vVertices[6] = D3DXVECTOR3(m_d3dxvMaximum.x, m_d3dxvMaximum.y, m_d3dxvMaximum.z);
	vVertices[7] = D3DXVECTOR3(m_d3dxvMaximum.x, m_d3dxvMaximum.y, m_d3dxvMinimum.z);
	m_d3dxvMinimum = D3DXVECTOR3(+FLT_MAX, +FLT_MAX, +FLT_MAX);
	m_d3dxvMaximum = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	//8���� �������� x, y, z ��ǥ�� �ּҰ��� �ִ밪�� ���Ѵ�.
	for (int i = 0; i < 8; i++)
	{
		//������ ��ȯ�Ѵ�.
		D3DXVec3TransformCoord(&vVertices[i], &vVertices[i], pmtxTransform);
		if (vVertices[i].x < m_d3dxvMinimum.x) m_d3dxvMinimum.x = vVertices[i].x;
		if (vVertices[i].y < m_d3dxvMinimum.y) m_d3dxvMinimum.y = vVertices[i].y;
		if (vVertices[i].z < m_d3dxvMinimum.z) m_d3dxvMinimum.z = vVertices[i].z;
		if (vVertices[i].x > m_d3dxvMaximum.x) m_d3dxvMaximum.x = vVertices[i].x;
		if (vVertices[i].y > m_d3dxvMaximum.y) m_d3dxvMaximum.y = vVertices[i].y;
		if (vVertices[i].z > m_d3dxvMaximum.z) m_d3dxvMaximum.z = vVertices[i].z;
	}
}

CMesh::CMesh(ID3D11Device *pd3dDevice)
{
	m_pd3dxvPositions = NULL;
	m_pnIndices = NULL;


	m_nBuffers = 0;
	m_pd3dPositionBuffer = NULL;
	//m_pd3dColorBuffer = NULL;
	m_ppd3dVertexBuffers = NULL;

	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_nSlot = 0;
	m_nStartVertex = 0;

	m_pd3dIndexBuffer = NULL;
	m_nIndices = 0;
	m_nStartIndex = 0;
	m_nBaseVertex = 0;
	m_nIndexOffset = 0;
	m_dxgiIndexFormat = DXGI_FORMAT_R32_UINT;

	m_pd3dRasterizerState = NULL;

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(+FLT_MAX, +FLT_MAX, +FLT_MAX);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	m_nReferences = 0;
}

CMesh::~CMesh()
{
	if (m_pd3dxvPositions) delete[] m_pd3dxvPositions;
	if (m_pnIndices) delete[] m_pnIndices;

	if (m_pd3dRasterizerState) m_pd3dRasterizerState->Release();
	if (m_pd3dPositionBuffer) m_pd3dPositionBuffer->Release();
	//if (m_pd3dColorBuffer) m_pd3dColorBuffer->Release();
	if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release();

	if (m_ppd3dVertexBuffers) delete[] m_ppd3dVertexBuffers;
	if (m_pnVertexStrides) delete[] m_pnVertexStrides;
	if (m_pnVertexOffsets) delete[] m_pnVertexOffsets;
}

void CMesh::AssembleToVertexBuffer(int nBuffers, ID3D11Buffer **ppd3dBuffers, UINT *pnBufferStrides, UINT *pnBufferOffsets)
{
	ID3D11Buffer **ppd3dNewVertexBuffers = new ID3D11Buffer*[m_nBuffers + nBuffers];
	UINT *pnNewVertexStrides = new UINT[m_nBuffers + nBuffers];
	UINT *pnNewVertexOffsets = new UINT[m_nBuffers + nBuffers];

	if (m_nBuffers > 0)
	{
		for (int i = 0; i < m_nBuffers; i++)
		{
			ppd3dNewVertexBuffers[i] = m_ppd3dVertexBuffers[i];
			pnNewVertexStrides[i] = m_pnVertexStrides[i];
			pnNewVertexOffsets[i] = m_pnVertexOffsets[i];
		}
		if (m_ppd3dVertexBuffers) delete[] m_ppd3dVertexBuffers;
		if (m_pnVertexStrides) delete[] m_pnVertexStrides;
		if (m_pnVertexOffsets) delete[] m_pnVertexOffsets;
	}

	for (int i = 0; i < nBuffers; i++)
	{
		ppd3dNewVertexBuffers[m_nBuffers + i] = ppd3dBuffers[i];
		pnNewVertexStrides[m_nBuffers + i] = pnBufferStrides[i];
		pnNewVertexOffsets[m_nBuffers + i] = pnBufferOffsets[i];
	}

	m_nBuffers += nBuffers;
	m_ppd3dVertexBuffers = ppd3dNewVertexBuffers;
	m_pnVertexStrides = pnNewVertexStrides;
	m_pnVertexOffsets = pnNewVertexOffsets;
}

void CMesh::UpdateIntanceBuffer(ID3D11Buffer *pd3dBuffers)
{
	
	if (m_ppd3dVertexBuffers[m_nBuffers - 1]) delete m_ppd3dVertexBuffers[m_nBuffers - 1];//�ν��Ͻ� ���� free���ְ�
	m_ppd3dVertexBuffers[m_nBuffers - 1] = pd3dBuffers;//���ο� �ν��Ͻ� ���۸� �־��ش�.

}


void CMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	//�޽��� ������ ���� ���� ���� ���۷� ǥ���ȴ�.
	pd3dDeviceContext->IASetVertexBuffers(m_nSlot, m_nBuffers, m_ppd3dVertexBuffers, m_pnVertexStrides, m_pnVertexOffsets);
	pd3dDeviceContext->IASetIndexBuffer(m_pd3dIndexBuffer, m_dxgiIndexFormat, m_nIndexOffset);
	pd3dDeviceContext->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dDeviceContext->RSSetState(m_pd3dRasterizerState);

	if (m_pd3dIndexBuffer)
		pd3dDeviceContext->DrawIndexed(m_nIndices, m_nStartIndex, m_nBaseVertex);
	else
		pd3dDeviceContext->Draw(m_nVertices, m_nStartVertex);
}
void CMesh::RenderInstanced(ID3D11DeviceContext *pd3dDeviceContext, int nInstances, int nStartInstance)
{
	//�ν��Ͻ��� ��� �Է� �����⿡ �޽��� ���� ���ۿ� �ν��Ͻ� ���� ���۰� ����ȴ�.
	pd3dDeviceContext->IASetVertexBuffers(m_nSlot, m_nBuffers, m_ppd3dVertexBuffers, m_pnVertexStrides, m_pnVertexOffsets);
	pd3dDeviceContext->IASetIndexBuffer(m_pd3dIndexBuffer, m_dxgiIndexFormat, m_nIndexOffset);
	pd3dDeviceContext->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dDeviceContext->RSSetState(m_pd3dRasterizerState);

	//��ü���� �ν��Ͻ����� �������Ѵ�. 
	if (m_pd3dIndexBuffer)
		pd3dDeviceContext->DrawIndexedInstanced(m_nIndices, nInstances, m_nStartIndex, m_nBaseVertex, nStartInstance);
	else
		pd3dDeviceContext->DrawInstanced(m_nVertices, nInstances, m_nStartVertex, nStartInstance);
}

int CMesh::CheckRayIntersection(D3DXVECTOR3 *pd3dxvRayPosition, D3DXVECTOR3 *pd3dxvRayDirection, MESHINTERSECTINFO *pd3dxIntersectInfo)
{
	//�� ��ǥ���� ������ ������(pd3dxvRayPosition)�� ������ �־��� �� �޽����� �浹 �˻縦 �Ѵ�.
	int nIntersections = 0;
	BYTE *pbPositions = (BYTE *)m_pd3dxvPositions + m_pnVertexOffsets[0];

	int nOffset = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? 3 : 1;
	//�޽��� ������Ƽ��(�ﰢ��)���� �����̴�. �ﰢ�� ����Ʈ�� ��� (������ ���� / 3) �Ǵ� (�ε����� ���� / 3), �ﰢ�� ��Ʈ���� ��� (������ ���� - 2) �Ǵ� (�ε����� ���� - 2)�̴�.
	int nPrimitives = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nVertices / 3) : (m_nVertices - 2);
	if (m_nIndices > 0) nPrimitives = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nIndices / 3) : (m_nIndices - 2);

	D3DXVECTOR3 v0, v1, v2;
	float fuHitBaryCentric, fvHitBaryCentric, fHitDistance, fNearHitDistance = FLT_MAX;
	/*�޽��� ��� ������Ƽ��(�ﰢ��)�鿡 ���Ͽ� ��ŷ �������� �浹�� �˻��Ѵ�. �浹�ϴ� ��� �ﰢ���� ã�� ������ ������(�����δ� ī�޶� ��ǥ���� ����)�� ���� ����� �ﰢ���� ã�´�.*/
	for (int i = 0; i < nPrimitives; i++)
	{
		v0 = *(D3DXVECTOR3 *)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i*nOffset) + 0]) : ((i*nOffset) + 0)) * m_pnVertexStrides[0]);
		v1 = *(D3DXVECTOR3 *)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i*nOffset) + 1]) : ((i*nOffset) + 1)) * m_pnVertexStrides[0]);
		v2 = *(D3DXVECTOR3 *)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i*nOffset) + 2]) : ((i*nOffset) + 2)) * m_pnVertexStrides[0]);
		if (D3DXIntersectTri(&v0, &v1, &v2, pd3dxvRayPosition, pd3dxvRayDirection, &fuHitBaryCentric, &fvHitBaryCentric, &fHitDistance))
		{
			if (fHitDistance < fNearHitDistance)
			{
				fNearHitDistance = fHitDistance;
				if (pd3dxIntersectInfo)
				{
					pd3dxIntersectInfo->m_dwFaceIndex = i;
					pd3dxIntersectInfo->m_fU = fuHitBaryCentric;
					pd3dxIntersectInfo->m_fV = fvHitBaryCentric;
					pd3dxIntersectInfo->m_fDistance = fHitDistance;
				}
			}
			nIntersections++;
		}
	}
	return(nIntersections);
}

//���� ������ �޽�
CMeshDiffused::CMeshDiffused(ID3D11Device *pd3dDevice) : CMesh(pd3dDevice)
{
	m_pd3dColorBuffer = NULL;
}

CMeshDiffused::~CMeshDiffused()
{
	if (m_pd3dColorBuffer) m_pd3dColorBuffer->Release();
}
//���� ������ �޽�
/*CTriangleMesh::CTriangleMesh(ID3D11Device *pd3dDevice) : CMesh(pd3dDevice)
{
	m_nVertices = 3;
	m_nStride = sizeof(CDiffusedVertex);
	m_nOffset = 0;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	/*����(�ﰢ���� ������)�� ������ �ð���� ������� ������, ���, �Ķ������� �����Ѵ�. D3DXCOLOR ��ũ�δ� RGBA(Red, Green, Blue, Alpha) 4���� �Ķ���͸� ����Ͽ� ������ ǥ���ϱ� ���Ͽ� ����Ѵ�. �� �Ķ���ʹ� 0.0~1.0 ������ �Ǽ����� ������.*/
	/*CDiffusedVertex pVertices[3];
	pVertices[0] = CDiffusedVertex(D3DXVECTOR3(0.0f, 0.5f, 0.0f), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
	pVertices[1] = CDiffusedVertex(D3DXVECTOR3(0.5f, -0.5f, 0.0f), D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f));
	pVertices[2] = CDiffusedVertex(D3DXVECTOR3(-0.5f, -0.5f, 0.0f), D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f));

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = m_nStride * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dVertexBuffer);

	CreateRasterizerState(pd3dDevice);
}

CTriangleMesh::~CTriangleMesh()
{
}

void CTriangleMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CMesh::Render(pd3dDeviceContext);
}

void CTriangleMesh::CreateRasterizerState(ID3D11Device *pd3dDevice)
{
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	//�����Ͷ����� �ܰ迡�� �ø�(���� ����)�� ���� �ʵ��� �����Ͷ����� ���¸� �����Ѵ�.
	d3dRasterizerDesc.CullMode = D3D11_CULL_NONE;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}*/



CCubeMeshDiffused::CCubeMeshDiffused(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth, D3DXCOLOR d3dxColor) : CMeshDiffused(pd3dDevice)
{
		m_nVertices = 8;
		m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

		float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

		//������ü �޽��� 2���� ���� ����(��ġ ���� ���ۿ� ���� ����)�� �����ȴ�.
		//������ü �޽��� ���� ����(��ġ ���� ����)�� �����Ѵ�.
		//D3DXVECTOR3 pd3dxvPositions[8];
		m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];

		m_pd3dxvPositions[0] = D3DXVECTOR3(-fx, +fy, -fz);
		m_pd3dxvPositions[1] = D3DXVECTOR3(+fx, +fy, -fz);
		m_pd3dxvPositions[2] = D3DXVECTOR3(+fx, +fy, +fz);
		m_pd3dxvPositions[3] = D3DXVECTOR3(-fx, +fy, +fz);
		m_pd3dxvPositions[4] = D3DXVECTOR3(-fx, -fy, -fz);
		m_pd3dxvPositions[5] = D3DXVECTOR3(+fx, -fy, -fz);
		m_pd3dxvPositions[6] = D3DXVECTOR3(+fx, -fy, +fz);
		m_pd3dxvPositions[7] = D3DXVECTOR3(-fx, -fy, +fz);

		D3D11_BUFFER_DESC d3dBufferDesc;
		ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
		d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
		d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		d3dBufferDesc.CPUAccessFlags = 0;
		D3D11_SUBRESOURCE_DATA d3dBufferData;
		ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
		d3dBufferData.pSysMem = m_pd3dxvPositions;
		pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);

		//������ü �޽��� ���� ����(���� ����)�� �����Ѵ�.
		D3DXCOLOR pd3dxColors[8];
		for (int i = 0; i < 8; i++) pd3dxColors[i] = d3dxColor + RANDOM_COLOR;

		d3dBufferDesc.ByteWidth = sizeof(D3DXCOLOR) * m_nVertices;
		d3dBufferData.pSysMem = pd3dxColors;
		pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dColorBuffer);

		ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dColorBuffer };
		UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXCOLOR) };
		UINT pnBufferOffsets[2] = { 0, 0 };
		AssembleToVertexBuffer(2, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

		m_nIndices = 18;

		//UINT pIndices[18];
		m_pnIndices = new UINT[m_nIndices];
		m_pnIndices[0] = 5; //5,6,4 - cw
		m_pnIndices[1] = 6; //6,4,7 - ccw
		m_pnIndices[2] = 4; //4,7,0 - cw
		m_pnIndices[3] = 7; //7,0,3 - ccw
		m_pnIndices[4] = 0; //0,3,1 - cw
		m_pnIndices[5] = 3; //3,1,2 - ccw
		m_pnIndices[6] = 1; //1,2,2 - cw 
		m_pnIndices[7] = 2; //2,2,3 - ccw
		m_pnIndices[8] = 2; //2,3,3 - cw  - Degenerated Index(2)
		m_pnIndices[9] = 3; //3,3,7 - ccw - Degenerated Index(3)
		m_pnIndices[10] = 3;//3,7,2 - cw  - Degenerated Index(3)
		m_pnIndices[11] = 7;//7,2,6 - ccw
		m_pnIndices[12] = 2;//2,6,1 - cw
		m_pnIndices[13] = 6;//6,1,5 - ccw
		m_pnIndices[14] = 1;//1,5,0 - cw
		m_pnIndices[15] = 5;//5,0,4 - ccw
		m_pnIndices[16] = 0;
		m_pnIndices[17] = 4;
	/*�ε��� ���۴� ������ü�� 6���� ��(�簢��)�� ���� ���� ������ ���´�. �ﰢ�� ����Ʈ�� ������ü�� ǥ���� ���̹Ƿ� �� ���� 2���� �ﰢ���� ������ �� �ﰢ���� 3���� ������ �ʿ��ϴ�. ��, �ε��� ���۴� ��ü 36(=6*2*3)���� �ε����� ������.*/
	/*m_nIndices = 36;

	UINT pIndices[36];
	//�� �ո�(Front) �簢���� ���� �ﰢ��
	pIndices[0] = 3; pIndices[1] = 1; pIndices[2] = 0;
	//�� �ո�(Front) �簢���� �Ʒ��� �ﰢ��
	pIndices[3] = 2; pIndices[4] = 1; pIndices[5] = 3;
	//�� ����(Top) �簢���� ���� �ﰢ��
	pIndices[6] = 0; pIndices[7] = 5; pIndices[8] = 4;
	//�� ����(Top) �簢���� �Ʒ��� �ﰢ��
	pIndices[9] = 1; pIndices[10] = 5; pIndices[11] = 0;
	//�� �޸�(Back) �簢���� ���� �ﰢ��
	pIndices[12] = 3; pIndices[13] = 4; pIndices[14] = 7;
	//�� �޸�(Back) �簢���� �Ʒ��� �ﰢ��
	pIndices[15] = 0; pIndices[16] = 4; pIndices[17] = 3;
	//�� �Ʒ���(Bottom) �簢���� ���� �ﰢ��
	pIndices[18] = 1; pIndices[19] = 6; pIndices[20] = 5;
	//�� �Ʒ���(Bottom) �簢���� �Ʒ��� �ﰢ��
	pIndices[21] = 2; pIndices[22] = 6; pIndices[23] = 1;
	//�� ����(Left) �簢���� ���� �ﰢ��
	pIndices[24] = 2; pIndices[25] = 7; pIndices[26] = 6;
	//�� ����(Left) �簢���� �Ʒ��� �ﰢ��
	pIndices[27] = 3; pIndices[28] = 7; pIndices[29] = 2;
	//�� ����(Right) �簢���� ���� �ﰢ��
	pIndices[30] = 6; pIndices[31] = 4; pIndices[32] = 5;
	//�� ����(Right) �簢���� �Ʒ��� �ﰢ��
	pIndices[33] = 7; pIndices[34] = 4; pIndices[35] = 6;
	*/
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(UINT) * m_nIndices;
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pnIndices;
	//�ε��� ���۸� �����Ѵ�.
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer);

	//���� ���� �����͸� ������ ���� �ּ����� �ִ����� �����Ѵ�. 
	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-fx, -fy, -fz);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(+fx, +fy, +fz);

	CreateRasterizerState(pd3dDevice);
}

CCubeMeshDiffused::~CCubeMeshDiffused()
{
}

/*void CCubeMesh::CreateRasterizerState(ID3D11Device *pd3dDevice)
{
	

	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	d3dRasterizerDesc.CullMode = D3D11_CULL_BACK;
	//d3dRasterizerDesc.CullMode = D3D11_CULL_NONE;
	//d3dRasterizerDesc.CullMode = D3D11_CULL_FRONT;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;

	//D3D11_FILL_WIREFRAME�� ������Ƽ���� ���θ� ĥ���� �ʰ� �������θ� �׸���. 	
	//d3dRasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}
void CCubeMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CMesh::Render(pd3dDeviceContext);
}*/


CAirplaneMeshDiffused::CAirplaneMeshDiffused(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth, D3DXCOLOR d3dxColor) : CMeshDiffused(pd3dDevice)
{
	m_nVertices = 24 * 3;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	//����� �޽��� 2���� ���� ����(��ġ ���� ���ۿ� ���� ����)�� �����ȴ�.
	//D3DXVECTOR3 pd3dxvPositions[24 * 3];
	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];

	float x1 = fx * 0.2f, y1 = fy * 0.2f, x2 = fx * 0.1f, y3 = fy * 0.3f, y2 = ((y1 - (fy - y3)) / x1) * x2 + (fy - y3);
	int i = 0;
	//Upper Plane
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +(fy + y3), -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -y1, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, 0.0f, -fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +(fy + y3), -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, 0.0f, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -y1, -fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x2, +y2, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -y3, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -y1, -fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x2, +y2, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -y1, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -y3, -fz);

	//Lower Plane
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +(fy + y3), +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, 0.0f, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -y1, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +(fy + y3), +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -y1, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, 0.0f, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x2, +y2, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -y1, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -y3, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x2, +y2, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -y3, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -y1, +fz);

	//Right Plane
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +(fy + y3), -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +(fy + y3), +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x2, +y2, -fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x2, +y2, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +(fy + y3), +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x2, +y2, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x2, +y2, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x2, +y2, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -y3, -fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -y3, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x2, +y2, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -y3, +fz);

	//Back/Right Plane
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -y1, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -y3, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -y3, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -y1, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+fx, -y3, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -y1, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, 0.0f, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -y1, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -y1, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, 0.0f, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(+x1, -y1, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, 0.0f, +fz);

	//Left Plane
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +(fy + y3), +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +(fy + y3), -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x2, +y2, -fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, +(fy + y3), +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x2, +y2, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x2, +y2, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x2, +y2, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x2, +y2, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -y3, -fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x2, +y2, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -y3, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -y3, +fz);

	//Back/Left Plane
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, 0.0f, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, 0.0f, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -y1, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(0.0f, 0.0f, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -y1, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -y1, -fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -y1, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -y1, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -y3, +fz);

	m_pd3dxvPositions[i++] = D3DXVECTOR3(-x1, -y1, -fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -y3, +fz);
	m_pd3dxvPositions[i++] = D3DXVECTOR3(-fx, -y3, -fz);

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pd3dxvPositions;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);

	D3DXCOLOR pd3dxColors[24 * 3];
	for (int j = 0; j < m_nVertices; j++) pd3dxColors[j] = d3dxColor + RANDOM_COLOR;
	pd3dxColors[0] = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pd3dxColors[3] = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pd3dxColors[6] = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	pd3dxColors[9] = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	pd3dxColors[12] = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pd3dxColors[15] = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pd3dxColors[18] = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	pd3dxColors[21] = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);

	d3dBufferDesc.ByteWidth = sizeof(D3DXCOLOR) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxColors;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dColorBuffer);

	ID3D11Buffer *ppd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dColorBuffer };
	UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXCOLOR) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer(2, ppd3dBuffers, pnBufferStrides, pnBufferOffsets);

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-fx, -fy, -fz);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(+fx, +fy, +fz);
}


CAirplaneMeshDiffused::~CAirplaneMeshDiffused()
{
}

CSphereMeshDiffused::CSphereMeshDiffused(ID3D11Device *pd3dDevice, float fRadius, int nSlices, int nStacks, D3DXCOLOR d3dxColor) : CMeshDiffused(pd3dDevice)
{
	m_nVertices = (nSlices * nStacks) * 3 * 2;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	//�� �޽��� 2���� ���� ����(��ġ ���� ���ۿ� ���� ����)�� �����ȴ�.
	//�� �޽��� ���� ����(��ġ ���� ����)�� �����Ѵ�.
	//D3DXVECTOR3 *pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	float theta_i, theta_ii, phi_j, phi_jj, fRadius_j, fRadius_jj, y_j, y_jj;
	for (int j = 0, k = 0; j < nStacks; j++)
	{
		phi_j = float(D3DX_PI / nStacks) * j;
		phi_jj = float(D3DX_PI / nStacks) * (j + 1);
		fRadius_j = fRadius * sinf(phi_j);
		fRadius_jj = fRadius * sinf(phi_jj);
		y_j = fRadius*cosf(phi_j);
		y_jj = fRadius*cosf(phi_jj);
		for (int i = 0; i < nSlices; i++)
		{
			theta_i = float(2 * D3DX_PI / nSlices) * i;
			theta_ii = float(2 * D3DX_PI / nSlices) * (i + 1);
			m_pd3dxvPositions[k++] = D3DXVECTOR3(fRadius_j*cosf(theta_i), y_j, fRadius_j*sinf(theta_i));
			m_pd3dxvPositions[k++] = D3DXVECTOR3(fRadius_jj*cosf(theta_i), y_jj, fRadius_jj*sinf(theta_i));
			m_pd3dxvPositions[k++] = D3DXVECTOR3(fRadius_j*cosf(theta_ii), y_j, fRadius_j*sinf(theta_ii));
			m_pd3dxvPositions[k++] = D3DXVECTOR3(fRadius_jj*cosf(theta_i), y_jj, fRadius_jj*sinf(theta_i));
			m_pd3dxvPositions[k++] = D3DXVECTOR3(fRadius_jj*cosf(theta_ii), y_jj, fRadius_jj*sinf(theta_ii));
			m_pd3dxvPositions[k++] = D3DXVECTOR3(fRadius_j*cosf(theta_ii), y_j, fRadius_j*sinf(theta_ii));
		}
	}

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pd3dxvPositions;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);

	//delete[] pd3dxvPositions;

	//�� �޽��� ���� ����(���� ����)�� �����Ѵ�.
	D3DXCOLOR *pd3dxColors = new D3DXCOLOR[m_nVertices];
	for (int i = 0; i < m_nVertices; i++) pd3dxColors[i] = d3dxColor + RANDOM_COLOR;

	d3dBufferDesc.ByteWidth = sizeof(D3DXCOLOR) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxColors;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dColorBuffer);

	delete[] pd3dxColors;

	ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dColorBuffer };
	UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXCOLOR) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer(2, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-fRadius, -fRadius, -fRadius);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(+fRadius, +fRadius, +fRadius);
}

CSphereMeshDiffused::~CSphereMeshDiffused()
{
}

/*void CSphereMesh::CreateRasterizerState(ID3D11Device *pd3dDevice)
{
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	d3dRasterizerDesc.CullMode = D3D11_CULL_BACK;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	d3dRasterizerDesc.DepthClipEnable = true;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
}*/

////////////////////////////////////////
CHeightMapGridMesh::CHeightMapGridMesh(ID3D11Device *pd3dDevice, int xStart, int zStart, int nWidth, int nLength, D3DXVECTOR3 d3dxvScale, D3DXCOLOR d3dxColor, void *pContext, DWORD dType, bool bReverse) : CMeshDiffused(pd3dDevice)
{
	m_dType = dType;
	m_bReverse = bReverse;

	//������ ����(����)�� ������ (nWidth * nLength)�̴�.
	m_nVertices = nWidth * nLength;
	//���ڴ� �ﰢ�� ��Ʈ������ �����Ѵ�.
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	D3DXCOLOR *pd3dxColors = new D3DXCOLOR[m_nVertices];
	D3DXVECTOR3 *pd3dxvNormals = new D3DXVECTOR3[m_nVertices];

	CHeightMap *pHeightMap = (CHeightMap *)pContext;

	m_nWidth = nWidth;
	m_nLength = nLength;
	m_d3dxvScale = d3dxvScale;

	float fHeight = 0.0f, fMinHeight = +FLT_MAX, fMaxHeight = -FLT_MAX;
	/*xStart�� zStart�� ������ ���� ��ġ(x-��ǥ�� z-��ǥ)�� ��Ÿ����. ������ ���ڵ��� ������ �迭�� ���� ���̱� ������ �������� �� ������ ���� ��ġ�� ��Ÿ���� ������ �ʿ��ϴ�. <�׸� 18>�� ������ ����(����)�� �����ϴ� ������ �����ش�.*/

	switch (m_dType)
	{
	case 0:
		for (int i = 0, z = zStart; z < (zStart + nLength); z++)
		{
			for (int x = xStart; x < (xStart + nWidth); x++, i++)
			{
				//������ ���̿� ������ ���� �����κ��� ���Ѵ�.
				fHeight = OnGetHeight(x, z, pContext);
				m_pd3dxvPositions[i] = D3DXVECTOR3((x*m_d3dxvScale.x), fHeight, (z*m_d3dxvScale.z));
				pd3dxColors[i] = OnGetColor(x, z, pContext) + d3dxColor;
				pd3dxvNormals[i] = pHeightMap->GetHeightMapNormal(x, z);
				if (fHeight < fMinHeight) fMinHeight = fHeight;
				if (fHeight > fMaxHeight) fMaxHeight = fHeight;
			}
		}
		break;
	case 1:
		for (int i = 0, y = zStart; y < (zStart + nLength); y++)
		{
			for (int z = xStart; z < (xStart + nWidth); z++, i++)
			{
				//������ ���̿� ������ ���� �����κ��� ���Ѵ�.
				fHeight = OnGetHeight(z, y, pContext);
				m_pd3dxvPositions[i] = D3DXVECTOR3(fHeight, (y*m_d3dxvScale.y), (z*m_d3dxvScale.z));
				pd3dxColors[i] = OnGetColor(z, y, pContext) + d3dxColor;
				pd3dxvNormals[i] = pHeightMap->GetHeightMapNormal(z, y);
				if (fHeight < fMinHeight) fMinHeight = fHeight;
				if (fHeight > fMaxHeight) fMaxHeight = fHeight;
			}
		}
		break;
	case 2:
		for (int i = 0, y = zStart; y < (zStart + nLength); y++)
		{
			for (int x = xStart; x < (xStart + nWidth); x++, i++)
			{
				//������ ���̿� ������ ���� �����κ��� ���Ѵ�.
				fHeight = OnGetHeight(x, y, pContext);
				m_pd3dxvPositions[i] = D3DXVECTOR3((y*m_d3dxvScale.x), (x*m_d3dxvScale.y), fHeight);
				pd3dxColors[i] = OnGetColor(x, y, pContext) + d3dxColor;
				pd3dxvNormals[i] = pHeightMap->GetHeightMapNormal(x, y);
				if (fHeight < fMinHeight) fMinHeight = fHeight;
				if (fHeight > fMaxHeight) fMaxHeight = fHeight;
			}
		}
		break;
	}

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pd3dxvPositions;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);

	d3dBufferDesc.ByteWidth = sizeof(D3DXCOLOR) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxColors;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dColorBuffer);
/*
	d3dBufferDesc.ByteWidth = sizeof(D3DXCOLOR) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxvNormals;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dNormalBuffer);*/

	ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dColorBuffer };
	UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXCOLOR) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer(2, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	delete[] pd3dxColors;

	m_nIndices = ((nWidth * 2)*(nLength - 1)) + ((nLength - 1) - 1);
	m_pnIndices = new UINT[m_nIndices];

	if (m_bReverse)
	{
		for (int j = 0, z = 0; z < nLength - 1; z++)
		{
			if ((z % 2) == 0)
			{
				for (int x = nWidth - 1; x >= 0; x--)
				{
					if ((x == nWidth - 1) && (z > 0)) m_pnIndices[j++] = (UINT)(x + (z * nWidth));
					m_pnIndices[j++] = (UINT)(x + (z * nWidth));
					m_pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
				}
			}
			else
			{
				for (int x = 0; x < nWidth; x++)
				{
					if (x == 0) m_pnIndices[j++] = (UINT)(x + (z * nWidth));
					m_pnIndices[j++] = (UINT)(x + (z * nWidth));
					m_pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
				}
			}
		}
	}
	else
	{
		for (int j = 0, z = 0; z < nLength - 1; z++)
		{
			if ((z % 2) == 0)
			{
				for (int x = 0; x < nWidth; x++)
				{
					if ((x == 0) && (z > 0)) m_pnIndices[j++] = (UINT)(x + (z * nWidth));
					m_pnIndices[j++] = (UINT)(x + (z * nWidth));
					m_pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
				}
			}
			else
			{
				for (int x = nWidth - 1; x >= 0; x--)
				{
					if (x == (nWidth - 1)) m_pnIndices[j++] = (UINT)(x + (z * nWidth));
					m_pnIndices[j++] = (UINT)(x + (z * nWidth));
					m_pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
				}
			}
		}
	}

	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(UINT) * m_nIndices;
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pnIndices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer);

	switch (m_dType)
	{
	case 0:
		m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(xStart*m_d3dxvScale.x, fMinHeight, zStart*m_d3dxvScale.z);
		m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3((xStart + nWidth)*m_d3dxvScale.x, fMaxHeight, (zStart + nLength)*m_d3dxvScale.z);
		break;
	case 1:
		m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(fMinHeight, zStart*m_d3dxvScale.y, xStart*m_d3dxvScale.z);
		m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(fMaxHeight, (zStart + nLength)*m_d3dxvScale.y, (xStart + nWidth)*m_d3dxvScale.z);
		break;
	case 2:
		m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(zStart*m_d3dxvScale.x, xStart*m_d3dxvScale.y, fMinHeight);
		m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3((zStart + nLength)*m_d3dxvScale.x, (xStart + nWidth)*m_d3dxvScale.y, fMaxHeight);
		break;
	}
}

CHeightMapGridMesh::~CHeightMapGridMesh()
{
}

float CHeightMapGridMesh::OnGetHeight(int x, int z, void *pContext)
{
	//���� �� ��ü�� ���� �� �̹����� �ȼ� ���� ������ ���̷� ��ȯ�Ѵ�. 
	CHeightMap *pHeightMap = (CHeightMap *)pContext;
	BYTE *pHeightMapImage = pHeightMap->GetHeightMapImage();
	D3DXVECTOR3 d3dxvScale = pHeightMap->GetScale();
	int cxTerrain = pHeightMap->GetHeightMapWidth();

	float fHeight = 0.0f;
	switch (m_dType)
	{
	case 0:
		fHeight = pHeightMapImage[x + (z*cxTerrain)] * d3dxvScale.y;
		break;
	case 1:
		fHeight = pHeightMapImage[x + (z*cxTerrain)] * d3dxvScale.x;
		break;
	case 2:
		fHeight = pHeightMapImage[x + (z*cxTerrain)] * d3dxvScale.z;
		break;
	}

	return(fHeight);
}
D3DXCOLOR CHeightMapGridMesh::OnGetColor(int x, int z, void *pContext)
{
	//������ ���� ����(�������� ��������� ����)�̴�.
	D3DXVECTOR3 d3dxvLightDirection = D3DXVECTOR3(-1.0f, 1.0f, 1.0f);
	D3DXVec3Normalize(&d3dxvLightDirection, &d3dxvLightDirection);
	CHeightMap *pHeightMap = (CHeightMap *)pContext;
	D3DXVECTOR3 d3dxvScale = pHeightMap->GetScale();
	//������ ����(����)�̴�. 
	D3DXCOLOR vIncidentLight(0.9f, 0.8f, 0.4f, 1.0f);
	/*���� (x, z)���� ������ �ݻ�Ǵ� ���� ���� (x, z)�� ���� ���Ϳ� ������ ���� ������ ����(cos)�� ������ 3���� �� (x+1, z), (x, z+1), (x+1, z+1)�� ���� ���Ϳ� ������ ���� ������ ������ ����Ͽ� ���Ѵ�. ���� (x, z)�� ������ ���� ����(����)�� �ݻ�Ǵ� ���� ���� ���̴�.*/

	float fScale = D3DXVec3Dot(&pHeightMap->GetHeightMapNormal(x, z), &d3dxvLightDirection);
	fScale += D3DXVec3Dot(&pHeightMap->GetHeightMapNormal(x + 1, z), &d3dxvLightDirection);
	fScale += D3DXVec3Dot(&pHeightMap->GetHeightMapNormal(x + 1, z + 1), &d3dxvLightDirection);
	fScale += D3DXVec3Dot(&pHeightMap->GetHeightMapNormal(x, z + 1), &d3dxvLightDirection);
	fScale = (fScale / 4.0f) + 0.05f;
	if (fScale > 1.0f) fScale = 1.0f;
	if (fScale < 0.25f) fScale = 0.25f;
	D3DXCOLOR d3dxColor = fScale * vIncidentLight;
	return(d3dxColor);
}

//���� ������ �޴� �޽�
CMeshIlluminated::CMeshIlluminated(ID3D11Device *pd3dDevice) : CMesh(pd3dDevice)
{
	m_pd3dNormalBuffer = NULL;
}

CMeshIlluminated::~CMeshIlluminated()
{
	if (m_pd3dNormalBuffer) m_pd3dNormalBuffer->Release();
}

void CMeshIlluminated::CalculateVertexNormal(D3DXVECTOR3 *pd3dxvNormals)
{
	switch (m_d3dPrimitiveTopology)
	{
		/*������Ƽ�갡 �ﰢ�� ����Ʈ�� �� �ε��� ���۰� �ִ� ���� ���� ��츦 �����Ͽ� ������ ���� ���͸� ����Ѵ�. �ε��� ���۸� ������� �ʴ� ��� �� ������ ���� ���ʹ� �� ������ ���Ե� �ﰢ���� ���� ���ͷ� ����Ѵ�. �ε��� ���۸� ����ϴ� ��� �� ������ ���� ���ʹ� �� ������ ���Ե� �ﰢ������ ���� ������ �������(���Ͽ�) ����Ѵ�.*/
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		if (m_pnIndices)
			SetAverageVertexNormal(pd3dxvNormals, (m_nIndices / 3), 3, false);
		else
			SetTriAngleListVertexNormal(pd3dxvNormals);
		break;
		/*������Ƽ�갡 �ﰢ�� ��Ʈ���� �� �� ������ ���� ���ʹ� �� ������ ���Ե� �ﰢ������ ���� ������ �������(���Ͽ�) ����Ѵ�.*/
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		SetAverageVertexNormal(pd3dxvNormals, (m_nIndices) ? (m_nIndices - 2) : (m_nVertices - 2), 1, true);
		break;
	default:
		break;
	}
}
//�ε��� ���۸� ������� �ʴ� �ﰢ�� ����Ʈ�� ���Ͽ� ������ ���� ���͸� ����Ѵ�.
void CMeshIlluminated::SetTriAngleListVertexNormal(D3DXVECTOR3 *pd3dxvNormals)
{
	/*�ﰢ��(������Ƽ��)�� ������ ���ϰ� �� �ﰢ���� ���� ���͸� ����ϰ� �ﰢ���� �����ϴ� �� ������ ���� ���ͷ� �����Ѵ�.*/
	int nPrimitives = m_nVertices / 3;
	for (int i = 0; i < nPrimitives; i++)
	{
		D3DXVECTOR3 d3dxvNormal = CalculateTriAngleNormal((i * 3 + 0), (i * 3 + 1), (i * 3 + 2));
		pd3dxvNormals[i * 3 + 0] = pd3dxvNormals[i * 3 + 1] = pd3dxvNormals[i * 3 + 2] = d3dxvNormal;
	}
}
//�ﰢ���� �� ������ ����Ͽ� �ﰢ���� ���� ���͸� ����Ѵ�.
D3DXVECTOR3 CMeshIlluminated::CalculateTriAngleNormal(UINT nIndex0, UINT nIndex1, UINT nIndex2)
{
	D3DXVECTOR3 d3dxvRandOffset(-4.0 + (rand() % 400) / 100.0, -4.0 + (rand() % 400) / 100.0, -4.0 + (rand() % 400) / 100.0);

	D3DXVECTOR3 d3dxvNormal;
	D3DXVECTOR3 d3dxvP0 = m_pd3dxvPositions[nIndex0] + d3dxvRandOffset;
	D3DXVECTOR3 d3dxvP1 = m_pd3dxvPositions[nIndex1] + d3dxvRandOffset;
	D3DXVECTOR3 d3dxvP2 = m_pd3dxvPositions[nIndex2] + d3dxvRandOffset;
	D3DXVECTOR3 d3dxvEdge1 = d3dxvP1 - d3dxvP0;
	D3DXVECTOR3 d3dxvEdge2 = d3dxvP2 - d3dxvP0;
	D3DXVec3Cross(&d3dxvNormal, &d3dxvEdge1, &d3dxvEdge2);
	D3DXVec3Normalize(&d3dxvNormal, &d3dxvNormal);
	return(d3dxvNormal);
}
/*������Ƽ�갡 �ε��� ���۸� ����ϴ� �ﰢ�� ����Ʈ �Ǵ� �ﰢ�� ��Ʈ���� ��� ������ ���� ���ʹ� �� ������ �����ϴ� �ﰢ���� ���� ���͵��� ������� ����Ѵ�.*/
void CMeshIlluminated::SetAverageVertexNormal(D3DXVECTOR3 *pd3dxvNormals, int nPrimitives, int nOffset, bool bStrip)
{
	for (int j = 0; j < m_nVertices; j++)
	{
		D3DXVECTOR3 d3dxvSumOfNormal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		for (int i = 0; i < nPrimitives; i++)
		{
			UINT nIndex0 = (bStrip) ? (((i % 2) == 0) ? (i*nOffset + 0) : (i*nOffset + 1)) : (i*nOffset + 0);
			if (m_pnIndices) nIndex0 = m_pnIndices[nIndex0];
			UINT nIndex1 = (bStrip) ? (((i % 2) == 0) ? (i*nOffset + 1) : (i*nOffset + 0)) : (i*nOffset + 1);
			if (m_pnIndices) nIndex1 = m_pnIndices[nIndex1];
			UINT nIndex2 = (m_pnIndices) ? m_pnIndices[i*nOffset + 2] : (i*nOffset + 2);
			if ((nIndex0 == j) || (nIndex1 == j) || (nIndex2 == j)) d3dxvSumOfNormal += CalculateTriAngleNormal(nIndex0, nIndex1, nIndex2);
		}
		D3DXVec3Normalize(&d3dxvSumOfNormal, &d3dxvSumOfNormal);
		pd3dxvNormals[j] = d3dxvSumOfNormal;
	}
}

CCubeMeshIlluminated::CCubeMeshIlluminated(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth) : CMeshIlluminated(pd3dDevice)
{
	m_nVertices = 8;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];

	m_pd3dxvPositions[0] = D3DXVECTOR3(-fx, +fy, -fz);
	m_pd3dxvPositions[1] = D3DXVECTOR3(+fx, +fy, -fz);
	m_pd3dxvPositions[2] = D3DXVECTOR3(+fx, +fy, +fz);
	m_pd3dxvPositions[3] = D3DXVECTOR3(-fx, +fy, +fz);
	m_pd3dxvPositions[4] = D3DXVECTOR3(-fx, -fy, -fz);
	m_pd3dxvPositions[5] = D3DXVECTOR3(+fx, -fy, -fz);
	m_pd3dxvPositions[6] = D3DXVECTOR3(+fx, -fy, +fz);
	m_pd3dxvPositions[7] = D3DXVECTOR3(-fx, -fy, +fz);

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pd3dxvPositions;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);

	m_nIndices = 36;
	m_pnIndices = new UINT[m_nIndices];

	m_pnIndices[0] = 3; m_pnIndices[1] = 1; m_pnIndices[2] = 0;
	m_pnIndices[3] = 2; m_pnIndices[4] = 1; m_pnIndices[5] = 3;
	m_pnIndices[6] = 0; m_pnIndices[7] = 5; m_pnIndices[8] = 4;
	m_pnIndices[9] = 1; m_pnIndices[10] = 5; m_pnIndices[11] = 0;
	m_pnIndices[12] = 3; m_pnIndices[13] = 4; m_pnIndices[14] = 7;
	m_pnIndices[15] = 0; m_pnIndices[16] = 4; m_pnIndices[17] = 3;
	m_pnIndices[18] = 1; m_pnIndices[19] = 6; m_pnIndices[20] = 5;
	m_pnIndices[21] = 2; m_pnIndices[22] = 6; m_pnIndices[23] = 1;
	m_pnIndices[24] = 2; m_pnIndices[25] = 7; m_pnIndices[26] = 6;
	m_pnIndices[27] = 3; m_pnIndices[28] = 7; m_pnIndices[29] = 2;
	m_pnIndices[30] = 6; m_pnIndices[31] = 4; m_pnIndices[32] = 5;
	m_pnIndices[33] = 7; m_pnIndices[34] = 4; m_pnIndices[35] = 6;

	D3DXVECTOR3 pd3dxvNormals[8];
	CalculateVertexNormal(pd3dxvNormals);

	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxvNormals;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dNormalBuffer);

	ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dNormalBuffer };
	UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR3) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer(2, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(UINT) * m_nIndices;
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pnIndices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer);

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-fx, -fy, -fz);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(+fx, +fy, +fz);
}

CCubeMeshIlluminated::~CCubeMeshIlluminated()
{
}

#define _WITH_INDEX_BUFFER

CSphereMeshIlluminated::CSphereMeshIlluminated(ID3D11Device *pd3dDevice, float fRadius, int nSlices, int nStacks) : CMeshIlluminated(pd3dDevice)
{
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fDeltaPhi = float(D3DX_PI / nStacks);
	float fDeltaTheta = float((2.0f * D3DX_PI) / nSlices);
	int k = 0;

#ifdef _WITH_INDEX_BUFFER
	m_nVertices = 2 + (nSlices * (nStacks - 1));
	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	D3DXVECTOR3 *pd3dxvNormals = new D3DXVECTOR3[m_nVertices];

	m_pd3dxvPositions[k] = D3DXVECTOR3(0.0f, +fRadius, 0.0f);
	D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
	float theta_i, phi_j;
	for (int j = 1; j < nStacks; j++)
	{
		phi_j = fDeltaPhi * j;
		for (int i = 0; i < nSlices; i++)
		{
			theta_i = fDeltaTheta * i;
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius*sinf(phi_j)*cosf(theta_i), fRadius*cosf(phi_j), fRadius*sinf(phi_j)*sinf(theta_i));
			D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
		}
	}
	m_pd3dxvPositions[k] = D3DXVECTOR3(0.0f, -fRadius, 0.0f);
	D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pd3dxvPositions;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);

	d3dBufferData.pSysMem = pd3dxvNormals;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dNormalBuffer);

	if (pd3dxvNormals) delete[] pd3dxvNormals;

	ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dNormalBuffer };
	UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR3) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer(2, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	k = 0;
	m_nIndices = (nSlices * 3) * 2 + (nSlices * (nStacks - 2) * 3 * 2);
	m_pnIndices = new UINT[m_nIndices];
	for (int i = 0; i < nSlices; i++)
	{
		m_pnIndices[k++] = 0;
		m_pnIndices[k++] = 1 + ((i + 1) % nSlices);
		m_pnIndices[k++] = 1 + i;
	}
	for (int j = 0; j < nStacks - 2; j++)
	{
		for (int i = 0; i < nSlices; i++)
		{
			m_pnIndices[k++] = 1 + (i + (j * nSlices));
			m_pnIndices[k++] = 1 + (((i + 1) % nSlices) + (j * nSlices));
			m_pnIndices[k++] = 1 + (i + ((j + 1) * nSlices));
			m_pnIndices[k++] = 1 + (i + ((j + 1) * nSlices));
			m_pnIndices[k++] = 1 + (((i + 1) % nSlices) + (j * nSlices));
			m_pnIndices[k++] = 1 + (((i + 1) % nSlices) + ((j + 1) * nSlices));
		}
	}
	for (int i = 0; i < nSlices; i++)
	{
		m_pnIndices[k++] = (m_nVertices - 1);
		m_pnIndices[k++] = ((m_nVertices - 1) - nSlices) + i;
		m_pnIndices[k++] = ((m_nVertices - 1) - nSlices) + ((i + 1) % nSlices);
	}

	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(UINT) * m_nIndices;
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pnIndices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer);
#else
	m_nVertices = (nSlices * 3) * 2 + (nSlices * (nStacks - 2) * 3 * 2);
	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	D3DXVECTOR3 *pd3dxvNormals = new D3DXVECTOR3[m_nVertices];

	float theta_i, theta_ii, phi_j = 0.0f, phi_jj = fDeltaPhi;
	for (int i = 0; i < nSlices; i++)
	{
		theta_i = fDeltaTheta * i;
		theta_ii = fDeltaTheta * (i + 1);
		m_pd3dxvPositions[k] = D3DXVECTOR3(0.0f, +fRadius, 0.0f);
		D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
		m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius*cosf(theta_i)*sinf(phi_jj), fRadius*cosf(phi_jj), fRadius*sinf(theta_i)*sinf(phi_jj));
		D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
		m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius*cosf(theta_ii)*sinf(phi_jj), fRadius*cosf(phi_jj), fRadius*sinf(theta_ii)*sinf(phi_jj));
		D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
	}

	for (int j = 1; j < nStacks - 1; j++)
	{
		phi_j = fDeltaPhi * j;
		phi_jj = fDeltaPhi * (j + 1);
		for (int i = 0; i < nSlices; i++)
		{
			theta_i = fDeltaTheta * i;
			theta_ii = fDeltaTheta * (i + 1);
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius*cosf(theta_i)*sinf(phi_j), fRadius*cosf(phi_j), fRadius*sinf(theta_i)*sinf(phi_j));
			D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius*cosf(theta_i)*sinf(phi_jj), fRadius*cosf(phi_jj), fRadius*sinf(theta_i)*sinf(phi_jj));
			D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius*cosf(theta_ii)*sinf(phi_j), fRadius*cosf(phi_j), fRadius*sinf(theta_ii)*sinf(phi_j));
			D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius*cosf(theta_i)*sinf(phi_jj), fRadius*cosf(phi_jj), fRadius*sinf(theta_i)*sinf(phi_jj));
			D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius*cosf(theta_ii)*sinf(phi_jj), fRadius*cosf(phi_jj), fRadius*sinf(theta_ii)*sinf(phi_jj));
			D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
			m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius*cosf(theta_ii)*sinf(phi_j), fRadius*cosf(phi_j), fRadius*sinf(theta_ii)*sinf(phi_j));
			D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
		}
	}
	phi_j = fDeltaPhi * (nStacks - 1);
	for (int i = 0; i < nSlices; i++)
	{
		theta_i = fDeltaTheta * i;
		theta_ii = fDeltaTheta * (i + 1);
		m_pd3dxvPositions[k] = D3DXVECTOR3(0.0f, -fRadius, 0.0f);
		D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
		m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius*cosf(theta_ii)*sinf(phi_j), fRadius*cosf(phi_j), fRadius*sinf(theta_ii)*sinf(phi_j));
		D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
		m_pd3dxvPositions[k] = D3DXVECTOR3(fRadius*cosf(theta_i)*sinf(phi_j), fRadius*cosf(phi_j), fRadius*sinf(theta_i)*sinf(phi_j));
		D3DXVec3Normalize(&pd3dxvNormals[k], &m_pd3dxvPositions[k]); k++;
	}

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pd3dxvPositions;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);

	d3dBufferData.pSysMem = pd3dxvNormals;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dNormalBuffer);

	if (pd3dxvNormals) delete[] pd3dxvNormals;

	ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dNormalBuffer };
	UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR3) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer(2, pd3dBuffers, pnBufferStrides, pnBufferOffsets);
#endif

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-fRadius, -fRadius, -fRadius);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(+fRadius, +fRadius, +fRadius);
}

CSphereMeshIlluminated::~CSphereMeshIlluminated()
{
}
