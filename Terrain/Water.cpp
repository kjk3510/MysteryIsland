#include "Water.h"
#include "RenderStates.h"



CWater::CWater() : mVB(0), mIB(0), mMapSRV(0), mTexOffset(0.0f, 0.0f), mVertexs(0), mTriangles(0),
mWidth(0), mDepth(0), mWavePos(0)
{
	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mWorld, I);
	XMStoreFloat4x4(&mTexTransform, I);
	mWorld._42 = -0.1f;

	mMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	mMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 32.0f);
}


CWater::~CWater()
{
	delete[] mWavePos;

	ReleaseCOM(mVB);
	ReleaseCOM(mIB);
	ReleaseCOM(mMapSRV);
}

void CWater::BuildWaterGeometryBuffers(ID3D11Device* pd3dDevice, UINT m, UINT n, float dx)
{
	mVertexs = m * n;
	mWidth = n * dx;
	mDepth = m * dx;

	float halfWidth = (n - 1)*dx*0.5f;
	float halfDepth = (m - 1)*dx*0.5f;

	std::vector<Vertex::Basic32> vertices;
	vertices.resize(mVertexs);

	for (UINT i = 0; i < m; ++i)
	{
		float z = halfDepth - i*dx;
		for (UINT j = 0; j < n; ++j)
		{
			float x = -halfWidth + j*dx;

			vertices[i*n + j].Pos = XMFLOAT3(x, 0.0f, z);
			mWavePos[i*n + j]     = XMFLOAT3(x, 0.0f, z);
			vertices[i*n + j].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			vertices[i*n + j].Tex.x = 0.5f + x / m * dx;
			vertices[i*n + j].Tex.y = 0.5f - z / n * dx;
		}
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * mVertexs;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(pd3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));

	mTriangles = (m - 1)*(n - 1) * 2;
	std::vector <UINT> indices;
	indices.resize(mTriangles * 3);

	int k = 0;
	for (UINT i = 0; i < m - 1; ++i)
	{
		for (DWORD j = 0; j < n - 1; ++j)
		{
			indices[k] = i*n + j;
			indices[k + 1] = i*n + j + 1;
			indices[k + 2] = (i + 1)*n + j;

			indices[k + 3] = (i + 1)*n + j;
			indices[k + 4] = i*n + j + 1;
			indices[k + 5] = (i + 1)*n + j + 1;

			k += 6; // next quad
		}
	}

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(pd3dDevice->CreateBuffer(&ibd, &iinitData, &mIB));
}

void CWater::InitWater(ID3D11Device* pd3dDevice, UINT m, UINT n, float dx)
{
	HR(D3DX11CreateShaderResourceViewFromFile(pd3dDevice,
		L"Textures/water2.dds", 0, 0, &mMapSRV, 0));

	delete[] mWavePos;
	mWavePos = new XMFLOAT3[m*n];

	BuildWaterGeometryBuffers(pd3dDevice, m, n, dx);
}

void CWater::UpdateWater(ID3D11DeviceContext* pd3dImmediateContext, float dt)
{
	XMMATRIX wavesScale = XMMatrixScaling(2.0f, 2.0f, 0.0f);

	//// Translate texture over time.
	static float fMovement = 0.0f;
	if (fMovement > 360.0f) fMovement = 0;
	fMovement += 0.5;
	//std::cout << sin(XMConvertToRadians(fMovement)) << std::endl;
	mTexOffset.y += sin(XMConvertToRadians(fMovement)) * dt * 0.1;
	mTexOffset.x += sin(XMConvertToRadians(fMovement)) * dt * 0.1;
	XMMATRIX wavesOffset = XMMatrixTranslation(mTexOffset.x, mTexOffset.y, 0.0f);

	// Combine scale and translation.
	XMStoreFloat4x4(&mTexTransform, wavesScale*wavesOffset);
}

void CWater::Draw(ID3D11DeviceContext* pd3dImmediateContext, Camera pCamera, DirectionalLight mDirLights[3])
{
	pd3dImmediateContext->IASetInputLayout(InputLayouts::Basic32);
	pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(Vertex::Basic32);
	UINT offset = 0;

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	Effects::BasicFX->SetDirLights(mDirLights);

	ID3DX11EffectTechnique* activeTech = Effects::BasicFX->Light3TexTech;

	D3DX11_TECHNIQUE_DESC techDesc;
	activeTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		//
		// Draw the hills.
		//
		pd3dImmediateContext->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
		pd3dImmediateContext->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);

		// Set per object constants.
		XMMATRIX world = XMLoadFloat4x4(&mWorld);
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world * pCamera.ViewProj();

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetTexTransform(XMLoadFloat4x4(&mTexTransform));
		Effects::BasicFX->SetMaterial(mMat);
		Effects::BasicFX->SetDiffuseMap(mMapSRV);


		pd3dImmediateContext->OMSetBlendState(RenderStates::TransparentBS, blendFactor, 0xffffffff);
		activeTech->GetPassByIndex(p)->Apply(0, pd3dImmediateContext);
		pd3dImmediateContext->DrawIndexed(3 * mTriangles, 0, 0);

		// Restore default blend state
		pd3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff);

		
	}
}