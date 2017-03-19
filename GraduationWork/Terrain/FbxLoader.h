#pragma once
#include <fbxsdk.h>
#include <vector>
#include <iostream>
#include <fstream>
#include "d3dApp.h"
#include "Vertex.h"
#include "SceneCache.h"
#include "GetPosition.h"
using namespace std;

enum Status
{
	UNLOADED, MUST_BE_LOADED, MUST_BE_REFRESHED, REFRESHED
};

class FbxLoader
{
private:

	int numTabs = 0;
	int IVertexCount;
	FbxManager * FManager;
	FbxIOSettings* IOManager;
	FbxImporter* Importer;
	FbxScene* Scene;
	FbxNode* RootNode;
	FbxMesh* IMesh;
	FbxVector4* IVertexArray;

	const char* UVName;
	FbxStringList FbxUVName;
	FbxLayerElementUV* layerUVs;

	//animation
	FbxAnimLayer* pAnimLayer;
	FbxAnimStack* pAnimStack;
	FbxAMatrix IDummyGlobalPosition;
	ShadingMede pShadingMode;
	FbxPose* pPose;
	FbxArray<FbxString*> mAnimStackNameArray;
	mutable FbxTime mStart, mStop, mCurrentTime;
	mutable FbxTime mCache_Start, mCache_Stop;
	mutable FbxTime pTime;
	mutable FbxTime FrameTime;
	mutable Status mStatus;
	bool Initialize = false;
	vector <Vertex::Basic32> CopyPos;
	int AnimStackCount;

	int AnimPosIndex = 0;
	vector<Vertex::Basic32> AnimPos;
	vector<vector<Vertex::Basic32>> AnimPosArr;
	vector<vector<vector<Vertex::Basic32>>> AllAnimPosArr;
	int AnimStackChangeIndex = 0;

public:

	bool AnimStop = false;

	bool AllAnimStop = false;

	FbxLoader();
	
	~FbxLoader();

	void LoadModel(const char * fileName, vector <Vertex::Basic32>& Pos, vector <UINT>& Index);

	FbxMatrix GetGeometryOffset(FbxNode* pNode);

	void DrawContinue(FbxTime& pTime, vector <Vertex::Basic32>& Pos);

	void DrawRecursive(FbxNode* pNode, FbxTime& pTime, FbxAnimLayer* pAnimLayer, FbxAMatrix& pParentGlobalPosition, ShadingMede pShadingMode, vector <Vertex::Basic32>& Pos);

	void AnimationChange(FbxNode* pNode, FbxTime& pTime, FbxAnimLayer* pAnimLayer, FbxAMatrix& GlobalPosition, ShadingMede pShadingMode, vector <Vertex::Basic32>& Pos);

	void ComputeShapeDeformation(FbxMesh *pMesh, FbxTime& pTime, FbxAnimLayer *pAnimLayer, vector <Vertex::Basic32>& Pos);

	void ComputeSkinDeformation(FbxAMatrix& pGlobalPosition, FbxMesh* pMesh, FbxTime& pTime, FbxVector4* pVertexArray);

	void ComputeLinearDeformation(FbxAMatrix& pGlobalPosition, FbxMesh* pMesh, FbxTime& pTime, FbxVector4* pVertexArray);

	void ComputeDualQuaternionDeformation(FbxAMatrix& pGlobalPosition, FbxMesh* pMesh, FbxTime& pTime, vector <Vertex::Basic32>& Pos);

	void ComputeClusterDeformation(FbxAMatrix& pGlobalPostion, FbxMesh* pMesh, FbxCluster* pCluster, FbxAMatrix& pVertexTransformMatrix, FbxTime pTime);

	void LoadNodeKeyframeAnimation(FbxNode* pNode);

	void LoadMeshSkeleton(FbxMesh* pMesh);

	void ReadVertexCacheData(FbxMesh* pMesh, FbxTime& pTime, vector <Vertex::Basic32> Pos);

	void MatrixScale(FbxAMatrix& pMatrix, double pValue);

	void MatrixAddToDiagonal(FbxAMatrix& pMatrix, double pValue);

	void MatrixAdd(FbxAMatrix& pDstMatrix, FbxAMatrix& pSrcMatrix);

	bool SetCurrentAnimStack(int pIndex);

	void OnTimerClick();

	const FbxArray<FbxString *> & GetAnimSatackNameArray() const { return mAnimStackNameArray; }

	FbxTime GetTime()
	{
		return mCurrentTime;
	}

	FbxTime GetFrameTime()
	{
		return FrameTime;
	}

	void UpdateVertexPosition(const FbxMesh* pMesh, FbxVector4* pVertices, vector <Vertex::Basic32>& Pos);

	void VertexInitialize(FbxNode* pNode, vector <Vertex::Basic32>& Pos, vector <UINT>& Index);

	void InitializeRecursive(FbxNode* pNode, FbxTime& pTime, FbxAMatrix& pParentGlobalPosition, vector <Vertex::Basic32>& Pos, vector<UINT>& Index);

	void DrawInitialize(FbxTime& pTime, vector <Vertex::Basic32>& Pos, vector <UINT>& Index);

	void SaveAllAnimInfomation(vector<vector<vector<Vertex::Basic32>>>& AllAnimPos);

	vector<vector<Vertex::Basic32>>& GetAnimInfo()
	{
		return AnimPosArr;
	}

	int GetAnimStackCount() const { return AnimStackCount; }

	int GetAnimStackChangeIndex() { return AnimStackChangeIndex; }

	void SetAnimStackChangeIndex(int value) { AnimStackChangeIndex = value; }

	//////////////////////////////////////////////////////////////

	void FileNameFinder();

	void PrintTabs();

	void PrintNode(FbxNode* pNode);

	void PrintAttribute(FbxNodeAttribute* pAttribute);

	FbxString GetAttributeTypeName(FbxNodeAttribute::EType type);
};

