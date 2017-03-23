#pragma once
#include <fbxsdk.h>
#include <vector>
#include <iostream>
#include "d3dApp.h"
#include "Vertex.h"
using namespace std;

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

public:
	FbxLoader();

	~FbxLoader();

	void LoadModel(const char * fileName, vector <Vertex::Basic32>& Pos, vector <UINT>& Index);

	FbxMatrix GetGeometryOffset(FbxNode* pNode);

	void DrawRecursive(FbxNode* pNode, FbxMatrix& pParentGlobalPosition, vector <Vertex::Basic32>& Pos, vector <UINT>& Index);

	void DrawMesh(FbxNode* pNode, vector <Vertex::Basic32>& Pos, vector <UINT>& Index);

	void LoadNodeKeyframeAnimation(FbxNode* pNode);

	void LoadMeshSkeleton(FbxMesh* pMesh);

	//////////////////////////////////////////////////////////////

	void FileNameFinder();

	void PrintTabs();

	void PrintNode(FbxNode* pNode);

	void PrintAttribute(FbxNodeAttribute* pAttribute);

	FbxString GetAttributeTypeName(FbxNodeAttribute::EType type);
};

