#pragma once
#include "d3dUtil.h"
#include "FbxLoader.h"
#include "Camera.h"

struct DirectionalLight;

class CGameObject
{
public:
	struct InitInfo
	{
		std::wstring  TextureName;
		Material Mat;
		XMFLOAT3 Pos;
	};
protected:
	ID3D11ShaderResourceView* mDiffuseMapSRV;
	Material mMat;
	Material mShadowMat;

	XMFLOAT4X4 mWorld;

	ID3D11Buffer* mVB;
	ID3D11Buffer* mIB;

public:
	CGameObject();
	~CGameObject();
	
	virtual void InitObject(ID3D11Device* pd3dDevice, const InitInfo& initInfo);
	virtual void UpdateObject(float dt);
	virtual void DrawObject(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3]);
	virtual void BuildGeometryBuffers(ID3D11Device* pd3dDevice);
};


class CPlayer : public CGameObject
{
private:
	Camera mCam;
	bool mFirstCamMode;
	
	float mMoveSpeed;
	float mAngle;

public:
	CPlayer();
	~CPlayer();

	Camera* GetCamera() { return &mCam; };
	XMFLOAT3 GetLookVector();
	XMFLOAT3 GetPosition();
	void InitObject(ID3D11Device* pd3dDevice, const InitInfo& initInfo);
	void UpdateObject(float dt, float playerHeight, float camHeight);
	void DrawObject(ID3D11DeviceContext* pd3dImmediateContext, const Camera& cam, DirectionalLight lights[3], PointLight pointLight, XMFLOAT4X4 shadow);
	void ShadowDraw(ID3D11DeviceContext* dc, XMFLOAT4X4 v, XMFLOAT4X4 p);
	void BuildGeometryBuffers(ID3D11Device* pd3dDevice);
	void RotateY(float angle);
	void InitCamera();
};

