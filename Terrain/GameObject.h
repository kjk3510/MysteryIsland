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
		char* FbxFileName;
		std::wstring  FbxTextureName;
		Material Mat;
		XMFLOAT4X4 Pos;
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
	
	void InitObject(ID3D11Device* device, const InitInfo& initInfo);
	void DrawObject(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3]);
};

class CPlayer : public CGameObject
{
private:
	Camera mCam;

public:
	CPlayer();
	~CPlayer();

	Camera* GetCamera() { return &mCam; };
	void InitObject(ID3D11Device* device, const InitInfo& initInfo);
	void DrawObject(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3]);
};

