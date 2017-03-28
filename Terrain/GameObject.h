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

#define FRONT XMFLOAT3(0.0f, 0.0f, 1.0f)
#define BACK  XMFLOAT3(0.0f, 0.0f, -1.0f)
#define RIGHT XMFLOAT3(1.0f, 0.0f, 0.0f)
#define LEFT  XMFLOAT3(-1.0f, 0.0f, 0.0f)

class CPlayer : public CGameObject
{
private:
	Camera mCam;
	
	float mMoveSpeed;
	float mAngle;

public:
	CPlayer();
	~CPlayer();

	Camera* GetCamera() { return &mCam; };
	XMFLOAT3 GetPosition();
	void InitObject(ID3D11Device* pd3dDevice, const InitInfo& initInfo);
	void UpdateObject(float dt, float height);
	void DrawObject(ID3D11DeviceContext* pd3dImmediateContext, const Camera& cam, DirectionalLight lights[3], PointLight pointLight);
	void BuildGeometryBuffers(ID3D11Device* pd3dDevice);
	void RotateY(float angle);
	//void Move(XMFLOAT3 dir, float dt);
	//void InputKeyboardMessage(float dt);
	//void SetRotateAngle(float angle) { mAngle = angle; }

	//const XMFLOAT3 GetPos() { return XMFLOAT3(mWorld._41, mWorld._42, mWorld._43); }
};

