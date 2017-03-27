#include "FbxLoader.h"

FbxLoader::FbxLoader() : FManager(NULL)
{
	FManager = FbxManager::Create();
	IOManager = FbxIOSettings::Create(FManager, IOSROOT);
	Importer = FbxImporter::Create(FManager, "");
	Scene = FbxScene::Create(FManager, "MyScene");
	RootNode = FbxNode::Create(Scene, "meshNode");
	IMesh = FbxMesh::Create(Scene, "mesh");
	FManager->SetIOSettings(IOManager);
	RootNode->SetNodeAttribute(IMesh);
	RootNode = Scene->GetRootNode();

	// Animation
	//AnimStack = FbxAnimStack::Create(Scene, "MyStack");
	//AnimLayer1 = FbxAnimLayer::Create(Scene, "Layer0");
	//AnimStack->AddMember(AnimLayer1);
	//AnimCurveNode = RootNode->LclTranslation.GetCurveNode(AnimLayer1, true);
}

FbxLoader::~FbxLoader()
{
	FManager->Destroy();
	Importer->Destroy();
}

FbxMatrix FbxLoader::GetGeometryOffset(FbxNode* pNode)
{
	const FbxVector4 IT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 IR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 IS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxMatrix(IT, IR, IS);
}

FbxString FbxLoader::GetAttributeTypeName(FbxNodeAttribute::EType type)
{
	switch (type) {
	case FbxNodeAttribute::eUnknown: return "unidentified";
	case FbxNodeAttribute::eNull: return "null";
	case FbxNodeAttribute::eMarker: return "marker";
	case FbxNodeAttribute::eSkeleton: return "skeleton";
	case FbxNodeAttribute::eMesh: return "mesh";
	case FbxNodeAttribute::eNurbs: return "nurbs";
	case FbxNodeAttribute::ePatch: return "patch";
	case FbxNodeAttribute::eCamera: return "camera";
	case FbxNodeAttribute::eCameraStereo: return "stereo";
	case FbxNodeAttribute::eCameraSwitcher: return "camera switcher";
	case FbxNodeAttribute::eLight: return "light";
	case FbxNodeAttribute::eOpticalReference: return "optical reference";
	case FbxNodeAttribute::eOpticalMarker: return "marker";
	case FbxNodeAttribute::eNurbsCurve: return "nurbs curve";
	case FbxNodeAttribute::eTrimNurbsSurface: return "trim nurbs surface";
	case FbxNodeAttribute::eBoundary: return "boundary";
	case FbxNodeAttribute::eNurbsSurface: return "nurbs surface";
	case FbxNodeAttribute::eShape: return "shape";
	case FbxNodeAttribute::eLODGroup: return "lodgroup";
	case FbxNodeAttribute::eSubDiv: return "subdiv";
	default: return "unknown";
	}
}

void FbxLoader::LoadModel(const char * fileName, vector <Vertex::Basic32>& Pos, vector <UINT>& Index)
{
	bool initialised = Importer->Initialize(fileName, -1, FManager->GetIOSettings());

	if (!initialised)
	{
		printf("Failed Importer"); return;
	}

	if (initialised)
	{
		printf("파일이 임포트됨 \n");
	}

	Importer->Import(Scene);

	RootNode = Scene->GetRootNode();

	DrawRecursive(RootNode, GetGeometryOffset(RootNode), Pos, Index);

	//PrintNode(RootNode);
}

void FbxLoader::DrawRecursive(FbxNode* pNode, FbxMatrix& pParentGlobalPosition, vector <Vertex::Basic32>& Pos, vector <UINT>& Index)
{
	FbxMatrix IGlobalPosition = pNode->EvaluateGlobalTransform(0);
	FbxNodeAttribute* INodeAttribute = pNode->GetNodeAttribute();

	if (INodeAttribute)
	{
		if (INodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			FbxMatrix IGeometryOffset = GetGeometryOffset(pNode);
			FbxMatrix IGlobalOffPosition = IGlobalPosition * IGeometryOffset;
			DrawMesh(pNode, Pos, Index);
		}
	}

	const int IChildCount = pNode->GetChildCount();

	for (int IChildIndex = 0; IChildIndex < IChildCount; ++IChildIndex)
	{
		DrawRecursive(pNode->GetChild(IChildIndex), IGlobalPosition, Pos, Index);
	}
}

void FbxLoader::DrawMesh(FbxNode* pNode, vector <Vertex::Basic32>& Pos, vector <UINT>& Index)
{
	IMesh = pNode->GetMesh();
	IVertexCount = IMesh->GetControlPointsCount(); 
	const int PolygonCount = IMesh->GetPolygonCount(); 
	const int PolygonVertexCount = 3;
	Vertex::Basic32 V;

	Pos.resize(PolygonCount * PolygonVertexCount);
	Index.resize(PolygonCount * PolygonVertexCount);

	//POS
	FbxVector4* FVpos;
	FbxVector4 FVpos2;
	XMFLOAT3 Vpos;

	//INDEX
	UINT v2;

	//UV
	IMesh->GetUVSetNames(FbxUVName);
	UVName = FbxUVName[0]; 
	FbxVector2 FUVPos;
	XMFLOAT2  UVPos;
	bool UnMappedUV = false;

	//NORMAL
	FbxVector4 FNormalPos;
	XMFLOAT3  NormalPos;

	int VertexCount = 0;
	unsigned int indx = 0;

	for (int lPolygonIndex = 0; lPolygonIndex < PolygonCount; ++lPolygonIndex)
	{
		const int VerticesCount = IMesh->GetPolygonSize(lPolygonIndex);
		for (int lVerticeIndex = 0; lVerticeIndex < VerticesCount; ++lVerticeIndex)
		{
			v2 = IMesh->GetPolygonVertex(lPolygonIndex, lVerticeIndex);
			Index[VertexCount] = indx;

			FVpos2 = IMesh->GetControlPointAt(v2);
			Vpos.x = static_cast<float>(FVpos2.mData[0]);	Vpos.y = static_cast<float>(FVpos2.mData[1]);	Vpos.z = static_cast<float>(FVpos2.mData[2]);
			Pos[VertexCount].Pos = Vpos;

			IMesh->GetPolygonVertexUV(lPolygonIndex, lVerticeIndex, UVName, FUVPos, UnMappedUV);
			UVPos.x = static_cast<float>(FUVPos.mData[0]);	UVPos.y = static_cast<float>(-FUVPos.mData[1]);
			Pos[VertexCount].Tex = UVPos;

			IMesh->GetPolygonVertexNormal(lPolygonIndex, lVerticeIndex, FNormalPos);
			NormalPos.x = static_cast<float>(FNormalPos.mData[0]);	NormalPos.y = static_cast<float>(FNormalPos.mData[1]);	NormalPos.z = static_cast<float>(FNormalPos.mData[2]);
			Pos[VertexCount].Normal = NormalPos;

			++VertexCount;
			++indx;
		}
	}
}

void FbxLoader::LoadNodeKeyframeAnimation(FbxNode* pNode)
{
	bool isAnimated = false;

	int numAnimations = Scene->GetSrcObjectCount(FbxAnimStack::ClassId);

	for (int animationIndex = 0; animationIndex < numAnimations; ++animationIndex)
	{
		FbxAnimStack* animStack = (FbxAnimStack*)Scene->GetSrcObject(FbxAnimStack::ClassId, animationIndex);
		FbxAnimEvaluator* animEvaluator = Scene->GetAnimationEvaluator();
		animStack->GetName(); //필요한 경우 애니메이션의 이름을 가져옵니다.

		int numLayers = animStack->GetMemberCount();
		for (int layerIndex = 0; layerIndex < numLayers; ++layerIndex)
		{
			FbxAnimLayer* animLayer = (FbxAnimLayer*)animStack->GetMember(layerIndex);
			animLayer->GetName(); //필요한 경우 레이어의 이름을 가져옵니다.

			FbxAnimCurve* translationCurve = pNode->LclTranslation.GetCurve(animLayer);
			FbxAnimCurve* rotationCurve = pNode->LclRotation.GetCurve(animLayer);
			FbxAnimCurve* scalingCurve = pNode->LclScaling.GetCurve(animLayer);

			if (translationCurve != 0 && rotationCurve !=0 && scalingCurve != 0)
			{
				int numKeys = scalingCurve->KeyGetCount();
				for (int keyIndex = 0; keyIndex < numKeys; ++keyIndex)
				{
					FbxTime frameTime = scalingCurve->KeyGetTime(keyIndex);
					FbxDouble3 translationVector = pNode->EvaluateLocalTranslation(frameTime);
					FbxDouble3 rotationVector = pNode->EvaluateLocalRotation(frameTime);
					FbxDouble3 scalingVector = pNode->EvaluateLocalScaling(frameTime);
					
					float translationX = static_cast<float>(translationVector.mData[0]);
					float translationY = static_cast<float>(translationVector.mData[1]);
					float translationZ = static_cast<float>(translationVector.mData[2]);
					float rotationX = static_cast<float>(rotationVector.mData[0]);
					float rotationY = static_cast<float>(rotationVector.mData[1]);
					float rotationZ = static_cast<float>(rotationVector.mData[2]);
					float scalingX = static_cast<float>(scalingVector.mData[0]);
					float scalingY = static_cast<float>(scalingVector.mData[1]);
					float scalingZ = static_cast<float>(scalingVector.mData[2]);

					float frameSconds = (float)frameTime.GetSecondDouble();
				}
			}

			else //만약 애니메이션 스텍이 존재하지 않는다면 디폴트 값으로 둔다.
			{
				FbxDouble3 translationVector = pNode->LclTranslation.Get();
				FbxDouble3 rotationVector = pNode->LclRotation.Get();
				FbxDouble3 scalingVector = pNode->LclScaling.Get();
				float x = (float)scalingVector[0];
				float y = (float)scalingVector[1];
				float z = (float)scalingVector[2];
			}
		}
	}
}

void FbxLoader::LoadMeshSkeleton(FbxMesh* pMesh)
{

}

void FbxLoader::FileNameFinder()
{
	if (RootNode)
	{
		for (int i = 0; i < RootNode->GetChildNameCount(); ++i)
		{
			PrintNode(RootNode->GetChild(i));
			printf("노드");
		}
	}
}

void FbxLoader::PrintTabs()
{
	for (int i = 0; i < numTabs; ++i)
		printf("\t");
}

void FbxLoader::PrintNode(FbxNode* pNode)
{
	//PrintTabs();
	const char* nodeName = pNode->GetName();
	//루트노드의 이름을 nodeName에 저장한다.
	FbxDouble3 translation = pNode->LclTranslation.Get();
	FbxDouble3 rotation = pNode->LclRotation.Get();
	FbxDouble3 scaling = pNode->LclScaling.Get();

	printf("<node name='%s' translation='(%f, %f, %f)' rotation='(%f, %f, %f)' scaling='(%f, %f, %f)'>\n",
		nodeName,
		translation[0], translation[1], translation[2],
		rotation[0], rotation[1], rotation[2],
		scaling[0], scaling[1], scaling[2]);

	numTabs++;

	for (int i = 0; i < pNode->GetNodeAttributeCount(); i++)
		PrintAttribute(pNode->GetNodeAttributeByIndex(i));

	for (int j = 0; j < pNode->GetChildCount(); j++)
		PrintNode(pNode->GetChild(j));

	numTabs--;
	PrintTabs();
	printf("</node> \n");
}

void FbxLoader::PrintAttribute(FbxNodeAttribute* pAttribute)
{
	if (!pAttribute) return;

	FbxString typeName = GetAttributeTypeName(pAttribute->GetAttributeType());
	FbxString attrName = pAttribute->GetName();
	//PrintTabs();
	printf("<attribute type = '%s' name = '%s'> \n", typeName.Buffer(), attrName.Buffer());
}
