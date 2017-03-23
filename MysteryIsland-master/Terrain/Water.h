#pragma once
#include "d3dUtil.h"
#include "Vertex.h"
#include "Camera.h"
#include "Effects.h"

class CWater
{
	ID3D11Buffer* mVB;
	ID3D11Buffer* mIB;

	ID3D11ShaderResourceView* mMapSRV;

	Material mMat;

	XMFLOAT4X4 mTexTransform;
	XMFLOAT4X4 mWorld;

	XMFLOAT2 mTexOffset;

	UINT mVertexs;
	UINT mTriangles;

	float mWidth;
	float mDepth;

	XMFLOAT3* mWavePos;
	
public:
	CWater();
	~CWater();
	
	void BuildWaterGeometryBuffers(ID3D11Device* pd3dDevice, UINT m, UINT n, float dx);
	void InitWater(ID3D11Device* pd3dDevice, UINT m, UINT n, float dx);
	void UpdateWater(ID3D11DeviceContext* pd3dImmediateContext, float dt);
	void Draw(ID3D11DeviceContext* pd3dImmediateContext, Camera pCamera, DirectionalLight mDirLights[3]);
};

