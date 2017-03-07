#pragma once
#include "d3dUtil.h"
#include "FbxLoader.h"
#include "Camera.h"

struct DirectionalLight;

ostream& operator<<(ostream& os, XMMATRIX m)
{
	os << m._11 << " " << m._12 << " " << m._13 << " " << m._14 << endl;
	os << m._21 << " " << m._22 << " " << m._23 << " " << m._24 << endl;
	os << m._31 << " " << m._32 << " " << m._33 << " " << m._34 << endl;
	os << m._41 << " " << m._42 << " " << m._43 << " " << m._44 << endl;

	return os;
}

class CGameObject
{
public:
	struct InitInfo
	{
		char* FbxFileName;
		std::wstring  FbxTextureName;
		Material Mat;
		XMFLOAT3 Pos;
	};
protected:
	FbxLoader FbxLoader;
	
	ID3D11ShaderResourceView* mDiffuseMapSRV;
	
	Material mMat;

	XMFLOAT4X4 mWorld;

	ID3D11Buffer* mVB;
	ID3D11Buffer* mIB;

	vector <Vertex::Basic32> Pos;
	vector <UINT> Index;

public:
	CGameObject();
	~CGameObject();
	
	virtual void InitObject(ID3D11Device* device, const InitInfo& initInfo);
	virtual void UpdateObject();
	virtual void DrawObject(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3]);
	virtual void Move(XMFLOAT3 dir, float dt);
};

#define FRONT XMFLOAT3(0.0f, 0.0f, 1.0f)
#define BACK  XMFLOAT3(0.0f, 0.0f, -1.0f)
#define RIGHT XMFLOAT3(1.0f, 0.0f, 0.0f)
#define LEFT  XMFLOAT3(-1.0f, 0.0f, 0.0f)

class CPlayer : public CGameObject
{
private:
	float mMoveSpeed;
	Camera mCam;
	float mAngle;

public:
	CPlayer();
	~CPlayer();

	Camera* GetCamera() { return &mCam; };
	void InitObject(ID3D11Device* device, const InitInfo& initInfo);
	void UpdateObject();
	void DrawObject(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3]);
	void Move(XMFLOAT3 dir, float dt);
	void InputKeyboardMessage(float dt);
	void SetRotateAngle(float angle) { mAngle = angle; }

	const XMFLOAT3 GetPos() { return XMFLOAT3(mWorld._41, mWorld._42, mWorld._43); }
};

