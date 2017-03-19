#include "FbxLoader.h"

FbxLoader::FbxLoader() : FManager(NULL), pAnimStack(NULL), pAnimLayer(NULL)
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
	mCache_Start = FBXSDK_TIME_INFINITE;
	mCache_Stop = FBXSDK_TIME_MINUS_INFINITE;
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
		printf("������ ����Ʈ�� \n");
	}

	Importer->Import(Scene);

	RootNode = Scene->GetRootNode();

	Scene->FillAnimStackNameArray(mAnimStackNameArray);

	//cout << pAnimLayer->GetName() << endl;;
	SetAnimStackChangeIndex(0);
	SetCurrentAnimStack(AnimStackChangeIndex);
	AnimStackCount = mAnimStackNameArray.GetCount();
	FrameTime.SetTime(0, 0, 0, 1, 0, Scene->GetGlobalSettings().GetTimeMode());
}

void FbxLoader::VertexInitialize(FbxNode* pNode, vector <Vertex::Basic32>& Pos, vector <UINT>& Index)
{
	FbxNodeAttribute* INodeAttribute = pNode->GetNodeAttribute();
	if (pNode->GetNodeAttribute())
	{
		if (INodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			IMesh = pNode->GetMesh();
			Vertex::Basic32 V;
			const int PolygonCount = IMesh->GetPolygonCount();
			const int PolygonVertexCount = 3;

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
	}
}

void FbxLoader::DrawContinue(FbxTime& pTime, vector <Vertex::Basic32>& Pos)
{
	DrawRecursive(RootNode, pTime, pAnimLayer, IDummyGlobalPosition, pShadingMode, Pos);
}

void FbxLoader::DrawInitialize(FbxTime& pTime, vector <Vertex::Basic32>& Pos, vector <UINT>& Index)
{
	InitializeRecursive(RootNode, pTime, IDummyGlobalPosition, Pos, Index);
	AnimPos = Pos; //�ؽ�����ǥ�� �븻��ǥ�� �ʱ�ȭ �����־�� �ִϸ��̼ǽ� �ؽ��Ŀ� �����ǥ�� ����ȴ�.
}

void FbxLoader::InitializeRecursive(FbxNode* pNode, FbxTime& pTime, FbxAMatrix& pParentGlobalPosition, vector <Vertex::Basic32>& Pos, vector<UINT>& Index)
{
	VertexInitialize(pNode, Pos, Index);

	FbxPose* pPose = NULL;
	FbxAMatrix IGlobalPosition = GetGlobalPosition(pNode, pTime, pPose, &pParentGlobalPosition);
	const int IChildCount = pNode->GetChildCount();
	for (int IChildIndex = 0; IChildIndex < IChildCount; ++IChildIndex)
	{
		InitializeRecursive(pNode->GetChild(IChildIndex), pTime, IGlobalPosition, Pos, Index);
	}
}

void FbxLoader::DrawRecursive(FbxNode* pNode, FbxTime& pTime, FbxAnimLayer* pAnimLayer, FbxAMatrix& pParentGlobalPosition, ShadingMede pShadingMode, vector <Vertex::Basic32>& Pos)
{
	FbxPose* pPose = NULL;
	FbxAMatrix IGlobalPosition = GetGlobalPosition(pNode, pTime, pPose, &pParentGlobalPosition);
	FbxNodeAttribute* INodeAttribute = pNode->GetNodeAttribute();

	if (pNode->GetNodeAttribute())
	{
		if (INodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			AnimationChange(pNode, pTime, pAnimLayer, pParentGlobalPosition, pShadingMode, Pos);
		}
	}

	const int IChildCount = pNode->GetChildCount();
	for (int IChildIndex = 0; IChildIndex < IChildCount; ++IChildIndex)
	{
		DrawRecursive(pNode->GetChild(IChildIndex), pTime, pAnimLayer, IGlobalPosition, pShadingMode,  Pos);
	}
}

void FbxLoader::AnimationChange(FbxNode* pNode, FbxTime& pTime, FbxAnimLayer* pAnimLayer, FbxAMatrix& GlobalPosition, ShadingMede pShadingMode, vector <Vertex::Basic32>& Pos)
{
	ofstream out("Crawler����.txt", ios::app);
	FbxVector4* IVertexArray = NULL;
	FbxMesh* IMesh = pNode->GetMesh();
	FbxSkin* ISkinDeformer = (FbxSkin *)IMesh->GetDeformer(0, FbxDeformer::eSkin);
	const bool IHasSkin = IMesh->GetDeformerCount(FbxDeformer::eSkin) > 0;

	const int IVertexCount = IMesh->GetControlPointsCount();
	IVertexArray = new FbxVector4[IVertexCount];
	memcpy(IVertexArray, IMesh->GetControlPoints(), IVertexCount * sizeof(FbxVector4));

	const int ISkinCount = IMesh->GetDeformerCount(FbxDeformer::eSkin);
	int IClusterCount = 0;
	for (int ISkinIndex = 0; ISkinIndex < ISkinCount; ++ISkinIndex)
	{
		IClusterCount += ((FbxSkin *)(IMesh->GetDeformer(ISkinIndex, FbxDeformer::eSkin)))->GetClusterCount();
	}

	if (IClusterCount)
	{
		ComputeSkinDeformation(GlobalPosition, IMesh, pTime, IVertexArray);
	}

	UpdateVertexPosition(IMesh, IVertexArray, Pos);
}

void FbxLoader::SaveAllAnimInfomation(vector<vector<vector<Vertex::Basic32>>>& AllAnimPos) //�����ڷ� 3���� ���͸� �ִ´�. ��ü���� �̰����� �޾ƾ��ϱ� ����
{
	if (!AnimStop) //���� �ִϸ��̼��� ������ ������������ �ƴ϶�� �ݺ�
	{
		OnTimerClick(); //�ð��� ��������ش�.
		DrawContinue(GetTime(), AnimPos);
		AnimPosArr.push_back(AnimPos); //�ϳ��� ������ �������� ��� 2���� ���Ϳ� �־��ش�.
		SaveAllAnimInfomation(AllAnimPos); //���� �������� �ִϸ��̼� ������ �����ϱ� ���� �ٽ� ȣ���Ͽ��ش�.
	}

	//�ϳ��� ������ �ִϸ��̼� ������ ��� �����ϰų�, ���� ������ ������ ������ �ƴ϶�� ȣ��
	else if (AnimStop && GetAnimStackChangeIndex() < GetAnimStackCount())
	{
		AllAnimPos.push_back(AnimPosArr);
		AnimPosArr.clear();
		SetAnimStackChangeIndex(GetAnimStackChangeIndex() + 1);
		SetCurrentAnimStack(GetAnimStackChangeIndex());
		AnimStop = false;
		cout << "�ִϸ��̼��� ������ ���Ͽ����ϴ�." << endl;
		SaveAllAnimInfomation(AllAnimPos);
	}

	else cout << "��� ������ �� ���߽��ϴ�." << endl;
}

//�޽��� ���� �� ������� ���� �迭�� �����մϴ�
void FbxLoader::ComputeShapeDeformation(FbxMesh *pMesh, FbxTime& pTime, FbxAnimLayer *pAnimLayer, vector <Vertex::Basic32>& Pos)
{
	CopyPos.resize(Pos.size());
	CopyPos = Pos;

	int IBlendShapeDeformerCount = pMesh->GetDeformerCount(FbxDeformer::eBlendShape);
	cout << IBlendShapeDeformerCount << endl;
	for (int IBlendShapeIndex = 0; IBlendShapeIndex < IBlendShapeDeformerCount; ++IBlendShapeIndex)
	{
		FbxBlendShape* IBlendShape = (FbxBlendShape*)pMesh->GetDeformer(IBlendShapeIndex, FbxDeformer::eBlendShape);
		int IBlendShapeChannelCount = IBlendShape->GetBlendShapeChannelCount();
		for (int IChannelIndex = 0; IChannelIndex < IBlendShapeChannelCount; ++IChannelIndex)
		{
			FbxBlendShapeChannel* IChannel = IBlendShape->GetBlendShapeChannel(IChannelIndex);
			if (IChannel)
			{
				FbxAnimCurve* IFCurve = pMesh->GetShapeChannel(IBlendShapeIndex, IChannelIndex, pAnimLayer);
				if (!IFCurve) continue;
				double IWeight = IFCurve->Evaluate(pTime);
				int ShapeCount = IChannel->GetTargetShapeCount();
				double* IFullWeight = IChannel->GetTargetShapeFullWeights();

				int IStartIndex = -1;
				int IEndIndex = -1;
				for (int IShapeIndex = 0; IShapeIndex < ShapeCount; ++IShapeIndex)
				{
					if (IWeight > 0 && IWeight <= IFullWeight[0])
					{
						IEndIndex = 0;
						break;
					}
					
					if (IWeight > IFullWeight[IShapeIndex] && IWeight < IFullWeight[IShapeIndex + 1])
					{
						IStartIndex = IShapeIndex;
						IEndIndex = IShapeIndex + 1;
						break;
					}
				}

				FbxShape* IStartShape = NULL;
				FbxShape* IEndShape = NULL;

				if (IStartIndex > -1)
				{
					IStartShape = IChannel->GetTargetShape(IStartIndex);
				}
				if (IEndIndex > -1)
				{
					IEndShape = IChannel->GetTargetShape(IEndIndex);
				}
				
				if (IStartIndex == -1 && IEndShape)
				{
					double IEndWeight = IFullWeight[0];
					IWeight = (IWeight / IEndWeight) * 100;
					CopyPos.resize(Pos.size());

					for (int j = 0; j < Pos.size(); ++j)
					{
						FbxVector4 Influence = (IEndShape->GetControlPoints()[j] - IStartShape->GetControlPoints()[j]) * IWeight * 0.01;
						CopyPos[j].Pos.x = Influence.mData[0];	
						CopyPos[j].Pos.y = Influence.mData[1];
						CopyPos[j].Pos.z = Influence.mData[2];
					}
				}

				else if (IStartShape && IEndShape)
				{
					double IStartWeight = IFullWeight[IStartIndex];
					double IEndWeight = IFullWeight[IEndIndex];
					IWeight = ((IWeight - IStartWeight) / (IEndWeight - IStartWeight)) * 100;
					CopyPos.resize(Pos.size());

					for (int j = 0; j < Pos.size(); ++j)
					{
						FbxVector4 influence = (IEndShape->GetControlPoints()[j] - IStartShape->GetControlPoints()[j]) * IWeight * 0.01;
						CopyPos[j].Pos.x = influence.mData[0];
						CopyPos[j].Pos.y = influence.mData[1];
						CopyPos[j].Pos.z = influence.mData[2];
					}
				}
			}
		}
	}
}

//�޽��� ��Ű�� ������ ���� �� ��ũ�� ���� ���� �迭�� �����մϴ�.
void FbxLoader::ComputeSkinDeformation(FbxAMatrix& pGlobalPosition, FbxMesh* pMesh, FbxTime& pTime, FbxVector4* pVertexArray)
{
	FbxSkin * ISkinDeformer = (FbxSkin *)pMesh->GetDeformer(0, FbxDeformer::eSkin);
	FbxSkin::EType ISkinningType = ISkinDeformer->GetSkinningType();

	if (ISkinningType == FbxSkin::eLinear || ISkinningType == FbxSkin::eRigid)
	{
		ComputeLinearDeformation(pGlobalPosition, pMesh, pTime, pVertexArray);
	}

	//else if (ISkinningType == FbxSkin::eDualQuaternion)
	//{
	//	ComputeDualQuaternionDeformation(pGlobalPosition, pMesh, pTime, Pos);
	//}

	//else if (ISkinningType == FbxSkin::eBlend)
	//{
	//	int IVertexCount = Pos.size();
	//	vector <Vertex::Basic32> LinearPos;
	//	vector <Vertex::Basic32> DQPos;
	//	LinearPos.resize(IVertexCount);
	//	DQPos.resize(IVertexCount);

	//	ComputeLinearDeformation(pGlobalPosition, pMesh, pTime, Pos);
	//	ComputeDualQuaternionDeformation(pGlobalPosition, pMesh, pTime, Pos);

	//	// ���� ����Ʈ�� ���� ��Ų�� ȥ���մϴ�.
	//	// ���� ���� = DQSVertex * ȥ�� ����ġ + LinearVertex * (1- ȥ�� ����ġ)
	//	// DQSVertex : ��� ���ʹϾ� ��Ű�� ������� ���� �� ����.
	//	// LinearVertex : �������� ���� ��Ű�� ������� ���� �� ����.
	//	int IBlendWeightsCount = ISkinDeformer->GetControlPointIndicesCount();
	//	for (int IBWindex = 0; IBWindex < IBlendWeightsCount; ++IBWindex)
	//	{
	//		double IBlendWeight = ISkinDeformer->GetControlPointBlendWeights()[IBWindex];
	//		DQPos[IBWindex].Pos.x *= IBlendWeight;	DQPos[IBWindex].Pos.y *= IBlendWeight;	DQPos[IBWindex].Pos.z *= IBlendWeight;
	//		LinearPos[IBWindex].Pos.x *= (1 - IBlendWeight);	LinearPos[IBWindex].Pos.y *= (1 - IBlendWeight);	LinearPos[IBWindex].Pos.z *= (1 - IBlendWeight);
	//		Pos[IBWindex].Pos.x = DQPos[IBWindex].Pos.x + LinearPos[IBWindex].Pos.x;
	//		Pos[IBWindex].Pos.y = DQPos[IBWindex].Pos.y + LinearPos[IBWindex].Pos.y;
	//		Pos[IBWindex].Pos.z = DQPos[IBWindex].Pos.z + LinearPos[IBWindex].Pos.z;
	//	}
	//}
}

// ���� ������� ���ؽ� �迭�� ����
void FbxLoader::ComputeLinearDeformation(FbxAMatrix& pGlobalPosition, FbxMesh* pMesh, FbxTime& pTime, FbxVector4* pVertexArray)
{
	// All the links must have the same link mode.
	FbxCluster::ELinkMode lClusterMode = ((FbxSkin*)pMesh->GetDeformer(0, FbxDeformer::eSkin))->GetCluster(0)->GetLinkMode();

	int lVertexCount = pMesh->GetControlPointsCount();
	FbxAMatrix* lClusterDeformation = new FbxAMatrix[lVertexCount];
	memset(lClusterDeformation, 0, lVertexCount * sizeof(FbxAMatrix));

	double* lClusterWeight = new double[lVertexCount];
	memset(lClusterWeight, 0, lVertexCount * sizeof(double));

	if (lClusterMode == FbxCluster::eAdditive)
	{
		for (int i = 0; i < lVertexCount; ++i)
		{
			lClusterDeformation[i].SetIdentity();
		}
	}

	// For all skins and all clusters, accumulate their deformation and weight
	// on each vertices and store them in lClusterDeformation and lClusterWeight.
	int lSkinCount = pMesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int lSkinIndex = 0; lSkinIndex<lSkinCount; ++lSkinIndex)
	{
		FbxSkin * lSkinDeformer = (FbxSkin *)pMesh->GetDeformer(lSkinIndex, FbxDeformer::eSkin);

		int lClusterCount = lSkinDeformer->GetClusterCount();
		for (int lClusterIndex = 0; lClusterIndex<lClusterCount; ++lClusterIndex)
		{
			FbxCluster* lCluster = lSkinDeformer->GetCluster(lClusterIndex);
			if (!lCluster->GetLink())
				continue;

			FbxAMatrix lVertexTransformMatrix;
			ComputeClusterDeformation(pGlobalPosition, pMesh, lCluster, lVertexTransformMatrix, pTime);

			int lVertexIndexCount = lCluster->GetControlPointIndicesCount();
			for (int k = 0; k < lVertexIndexCount; ++k)
			{
				int lIndex = lCluster->GetControlPointIndices()[k];

				// Sometimes, the mesh can have less points than at the time of the skinning
				// because a smooth operator was active when skinning but has been deactivated during export.
				if (lIndex >= lVertexCount)
					continue;

				double lWeight = lCluster->GetControlPointWeights()[k];

				if (lWeight == 0.0)
				{
					continue;
				}

				// Compute the influence of the link on the vertex.
				FbxAMatrix lInfluence = lVertexTransformMatrix;
				MatrixScale(lInfluence, lWeight / 100);

				if (lClusterMode == FbxCluster::eAdditive)
				{
					// Multiply with the product of the deformations on the vertex.
					MatrixAddToDiagonal(lInfluence, 1.0 - lWeight);
					lClusterDeformation[lIndex] = lInfluence * lClusterDeformation[lIndex];

					// Set the link to 1.0 just to know this vertex is influenced by a link.
					lClusterWeight[lIndex] = 1.0;
				}
				else // lLinkMode == FbxCluster::eNormalize || lLinkMode == FbxCluster::eTotalOne
				{
					// Add to the sum of the deformations on the vertex.
					MatrixAdd(lClusterDeformation[lIndex], lInfluence);

					// Add to the sum of weights to either normalize or complete the vertex.
					lClusterWeight[lIndex] += lWeight;
				}
			}//For each vertex                      
		}//lClusterCount
	}

	//Actually deform each vertices here by information stored in lClusterDeformation and lClusterWeight
	for (int i = 0; i < lVertexCount; i++)
	{
		FbxVector4 lSrcVertex = pVertexArray[i];
		FbxVector4& lDstVertex = pVertexArray[i];
		double lWeight = lClusterWeight[i];

		// Deform the vertex if there was at least a link with an influence on the vertex,
		if (lWeight != 0.0)
		{
			lDstVertex = lClusterDeformation[i].MultT(lSrcVertex);
			if (lClusterMode == FbxCluster::eNormalize)
			{
				// In the normalized link mode, a vertex is always totally influenced by the links. 
				lDstVertex /= lWeight;
			}
			else if (lClusterMode == FbxCluster::eTotalOne)
			{
				// In the total 1 link mode, a vertex can be partially influenced by the links. 
				lSrcVertex *= (1.0 - lWeight);
				lDstVertex += lSrcVertex;
			}
		}
	}

	delete[] lClusterDeformation;
	delete[] lClusterWeight;
}

// Dual Quaternion Skinning ������� ���ؽ� �迭�� ����
void FbxLoader::ComputeDualQuaternionDeformation(FbxAMatrix& pGlobalPosition, FbxMesh* pMesh, FbxTime& pTime, vector <Vertex::Basic32>& Pos)
{
	FbxCluster::ELinkMode IClusterMode = ((FbxSkin*)pMesh->GetDeformer(0, FbxDeformer::eSkin))->GetCluster(0)->GetLinkMode();

	int VertexCount = Pos.size();
	int ISkinCount = pMesh->GetDeformerCount(FbxDeformer::eSkin);

	FbxDualQuaternion* IDQClusterDeformation = new FbxDualQuaternion[IVertexCount];
	memset(IDQClusterDeformation, 0, IVertexCount * sizeof(FbxDualQuaternion));

	double* IClusterWeight = new double[IVertexCount];
	memset(IClusterWeight, 0, IVertexCount * sizeof(double));

	//��� ��Ų �� ��� Ŭ�����Ϳ� ���� �� �������� ���� �� ����ġ�� �����Ͽ� lClusterDeformation �� lClusterWeight�� �����մϴ�.
	for (int ISkinIndex = 0; ISkinIndex < ISkinCount; ++ISkinIndex)
	{
		FbxSkin* ISkinDeformer = (FbxSkin*)pMesh->GetDeformer(ISkinIndex, FbxDeformer::eSkin);
		int IClusterCount = ISkinDeformer->GetClusterCount();
		for (int IClusterIndex = 0; IClusterIndex < IClusterCount; ++IClusterIndex)
		{
			FbxCluster* ICluster = ISkinDeformer->GetCluster(IClusterIndex);
			if (!ICluster->GetLink())
				continue;

			FbxAMatrix IVertexTransformMatrix;
			ComputeClusterDeformation(pGlobalPosition, pMesh, ICluster, IVertexTransformMatrix, pTime);

			FbxQuaternion IQ = IVertexTransformMatrix.GetQ();
			FbxVector4 IT = IVertexTransformMatrix.GetT();
			FbxDualQuaternion IDualQuaternion(IQ, IT);

			int IVertexIndexCount = ICluster->GetControlPointIndicesCount();
			for (int k = 0; k < IVertexIndexCount; ++k)
			{
				int IIndex = ICluster->GetControlPointIndices()[k];
				//������ �� �� �ε巯�� �����ڰ� Ȱ��ȭ �Ǿ����� �������� �߿� ��Ȱ��ȭ �� ��찡 �ֱ� ������ ��Ų �� ������ �޽��� ����Ʈ�� ���� �� �ֽ��ϴ�.
				if (IIndex >= IVertexCount)
					continue;

				double IWeight = ICluster->GetControlPointWeights()[k];

				if (IWeight == 0.0)
					continue;

				//������ ���� ��ũ�� ������ ����մϴ�.
				FbxDualQuaternion IInfluence = IDualQuaternion * IWeight;
				if (IClusterMode == FbxCluster::eAdditive)
				{
					//������ʹϾ��� ������ �޽��ϴ�.
					IDQClusterDeformation[IIndex] = IInfluence;

					//�� ������ ��ũ�� ������ �޴����� �˱� ���� ��ũ�� 1.0���� �����Ͻʽÿ�.
					IClusterWeight[IIndex] = 1.0;
				}

				else
				{
					if (IClusterIndex == 0)
					{
						IDQClusterDeformation[IIndex] = IInfluence;
					}

					else
					{
						double ISign = IDQClusterDeformation[IIndex].GetFirstQuaternion().DotProduct(IDualQuaternion.GetFirstQuaternion());
						if (ISign >= 0.0)
						{
							IDQClusterDeformation[IIndex] += IInfluence;
						}
						else
						{
							IDQClusterDeformation[IIndex] -= IInfluence;
						}
					}
					IClusterWeight[IIndex] += IWeight;
				}
			}
		}
	}
}

void FbxLoader::ComputeClusterDeformation(FbxAMatrix& pGlobalPostion, FbxMesh* pMesh, FbxCluster* pCluster, FbxAMatrix& pVertexTransformMatrix, FbxTime pTime)
{
	FbxPose* pPose = NULL;
	FbxCluster::ELinkMode lClusterMode = pCluster->GetLinkMode();

	FbxAMatrix lReferenceGlobalInitPosition;
	FbxAMatrix lReferenceGlobalCurrentPosition;
	FbxAMatrix lAssociateGlobalInitPosition;
	FbxAMatrix lAssociateGlobalCurrentPosition;
	FbxAMatrix lClusterGlobalInitPosition;
	FbxAMatrix lClusterGlobalCurrentPosition;

	FbxAMatrix lReferenceGeometry;
	FbxAMatrix lAssociateGeometry;
	FbxAMatrix lClusterGeometry;

	FbxAMatrix lClusterRelativeInitPosition;
	FbxAMatrix lClusterRelativeCurrentPositionInverse;

	if (lClusterMode == FbxCluster::eAdditive && pCluster->GetAssociateModel())
	{
		pCluster->GetTransformAssociateModelMatrix(lAssociateGlobalInitPosition);
		// Geometric transform of the model
		lAssociateGeometry = GetGeometry(pCluster->GetAssociateModel());
		lAssociateGlobalInitPosition *= lAssociateGeometry;
		lAssociateGlobalCurrentPosition = GetGlobalPosition(pCluster->GetAssociateModel(), pTime, pPose);

		pCluster->GetTransformMatrix(lReferenceGlobalInitPosition);
		// Multiply lReferenceGlobalInitPosition by Geometric Transformation
		lReferenceGeometry = GetGeometry(pMesh->GetNode());
		lReferenceGlobalInitPosition *= lReferenceGeometry;
		lReferenceGlobalCurrentPosition = pGlobalPostion;

		// Get the link initial global position and the link current global position.
		pCluster->GetTransformLinkMatrix(lClusterGlobalInitPosition);
		// Multiply lClusterGlobalInitPosition by Geometric Transformation
		lClusterGeometry = GetGeometry(pCluster->GetLink());
		lClusterGlobalInitPosition *= lClusterGeometry;
		lClusterGlobalCurrentPosition = GetGlobalPosition(pCluster->GetLink(), pTime, pPose);

		// Compute the shift of the link relative to the reference.
		//ModelM-1 * AssoM * AssoGX-1 * LinkGX * LinkM-1*ModelM
		pVertexTransformMatrix = lReferenceGlobalInitPosition.Inverse() * lAssociateGlobalInitPosition * lAssociateGlobalCurrentPosition.Inverse() *
			lClusterGlobalCurrentPosition * lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;
	}
	else
	{
		pCluster->GetTransformMatrix(lReferenceGlobalInitPosition);
		lReferenceGlobalCurrentPosition = pGlobalPostion;
		// Multiply lReferenceGlobalInitPosition by Geometric Transformation
		lReferenceGeometry = GetGeometry(pMesh->GetNode());
		lReferenceGlobalInitPosition *= lReferenceGeometry;

		// Get the link initial global position and the link current global position.
		pCluster->GetTransformLinkMatrix(lClusterGlobalInitPosition);
		lClusterGlobalCurrentPosition = GetGlobalPosition(pCluster->GetLink(), pTime, pPose);

		// Compute the initial position of the link relative to the reference.
		lClusterRelativeInitPosition = lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;

		// Compute the current position of the link relative to the reference.
		lClusterRelativeCurrentPositionInverse = lReferenceGlobalCurrentPosition.Inverse() * lClusterGlobalCurrentPosition;

		// Compute the shift of the link relative to the reference.
		pVertexTransformMatrix = lClusterRelativeCurrentPositionInverse * lClusterRelativeInitPosition;
	}
}

void FbxLoader::MatrixScale(FbxAMatrix& pMatrix, double pValue)
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			pMatrix[i][j] *= pValue;
		}
	}
}

//����� �밢���� �ִ� ��� ��ҿ� ���� �߰��մϴ�.
void FbxLoader::MatrixAddToDiagonal(FbxAMatrix& pMatrix, double pValue)
{
	pMatrix[0][0] += pValue;
	pMatrix[1][1] += pValue;
	pMatrix[2][2] += pValue;
	pMatrix[3][3] += pValue;
}

void FbxLoader::MatrixAdd(FbxAMatrix& pDstMatrix, FbxAMatrix& pSrcMatrix)
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			pDstMatrix[i][j] += pSrcMatrix[i][j];
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
		//cout << animStack->GetName() << endl; //�ʿ��� ��� �ִϸ��̼��� �̸��� �����ɴϴ�.

		int numLayers = animStack->GetMemberCount();
		for (int layerIndex = 0; layerIndex < numLayers; ++layerIndex)
		{
			FbxAnimLayer* animLayer = (FbxAnimLayer*)animStack->GetMember(layerIndex);
			//cout << animLayer->GetName() << endl; //�ʿ��� ��� ���̾��� �̸��� �����ɴϴ�.

			FbxAnimCurve* translationCurve = pNode->LclTranslation.GetCurve(animLayer);
			FbxAnimCurve* rotationCurve = pNode->LclRotation.GetCurve(animLayer);
			FbxAnimCurve* scalingCurve = pNode->LclScaling.GetCurve(animLayer);

			if (translationCurve != 0)
			{
				int numKeys = translationCurve->KeyGetCount();
				for (int keyIndex = 0; keyIndex < numKeys; ++keyIndex)
				{
					FbxTime frameTime = translationCurve->KeyGetTime(keyIndex);
					FbxDouble3 translationVector = pNode->EvaluateLocalTranslation(frameTime);
					float translationX = static_cast<float>(translationVector[0]);	float translationY = static_cast<float>(translationVector[1]);	float translationZ = static_cast<float>(translationVector[2]);
					float frameSeconds = (float)frameTime.GetSecondDouble();
				}
			}

			if (rotationCurve != 0)
			{
				int numKeys = rotationCurve->KeyGetCount();
				for (int keyIndex = 0; keyIndex < numKeys; ++keyIndex)
				{
					FbxTime frameTime = rotationCurve->KeyGetTime(keyIndex);
					FbxDouble3 rotationVector = pNode->EvaluateLocalRotation(frameTime);
					float rotationX = static_cast<float>(rotationVector[0]);	float rotationY = static_cast<float>(rotationVector[1]);		float roationZ = static_cast<float>(rotationVector[2]);
				}
			}

			if (scalingCurve != 0)
			{
				int numKeys = scalingCurve->KeyGetCount();
				for (int keyIndex = 0; keyIndex < numKeys; ++keyIndex)
				{
					FbxTime frameTime = scalingCurve->KeyGetTime(keyIndex);
					FbxDouble3 scalingVector = pNode->EvaluateLocalScaling(frameTime);
					float scalingX = static_cast<float>(scalingVector[0]);	float scalingY = static_cast<float>(scalingVector[1]);	float scalingZ = static_cast<float>(scalingVector[2]);
					float frameSconds = (float)frameTime.GetSecondDouble();
				}
			}
		}
	}
}

void FbxLoader::LoadMeshSkeleton(FbxMesh* pMesh)
{
	int numDeformers = pMesh->GetDeformerCount();
	FbxSkin* skin = (FbxSkin*)pMesh->GetDeformer(0, FbxDeformer::eSkin);
	if (skin != 0)
	{
		int boneCount = skin->GetClusterCount();
		for (int boneIndex = 0; boneIndex < boneCount; ++boneIndex)
		{
			FbxCluster* cluster = skin->GetCluster(boneIndex);
			FbxNode* bone = cluster->GetLink();

			FbxAMatrix bindPoseMatrix;
			cluster->GetTransformLinkMatrix(bindPoseMatrix);

			int *boneVertexIndices = cluster->GetControlPointIndices();
			double *boneVertexWeights = cluster->GetControlPointWeights();

			//���� ������ �޴� ��� ������ �ݺ��մϴ�.
			int numBoneVertexIndices = cluster->GetControlPointIndicesCount();
			for (int boneVertexIndex = 0; boneVertexIndex < numBoneVertexIndices; ++boneVertexIndex)
			{
				int boneVertexIndex2 = boneVertexIndices[boneVertexIndex];
				float boneWeight = static_cast<float>(boneVertexWeights[boneVertexIndex2]);
			}
		}
	}
}

void FbxLoader::ReadVertexCacheData(FbxMesh* pMesh, FbxTime& pTime, vector <Vertex::Basic32> Pos)
{
	FbxVertexCacheDeformer* IDeformer = static_cast<FbxVertexCacheDeformer*>(pMesh->GetDeformer(0, FbxDeformer::eVertexCache));
	FbxCache* ICache = IDeformer->GetCache();
	unsigned int IVertexCount = (unsigned int)pMesh->GetControlPointsCount();
	bool IReadSucceed = false;
	double* IReadBuf = new double[3 * IVertexCount];

	IReadSucceed = ICache->Read((unsigned int)pTime.GetFrameCount(), IReadBuf, IVertexCount);

	if (IReadSucceed)
	{
		unsigned int IReadBufIndex = 0;

		while (IReadBufIndex < Pos.size())
		{
			Pos[IReadBufIndex].Pos.x = IReadBuf[IReadBufIndex]; ++IReadBufIndex;
			Pos[IReadBufIndex].Pos.y = IReadBuf[IReadBufIndex]; ++IReadBufIndex;
			Pos[IReadBufIndex].Pos.z = IReadBuf[IReadBufIndex]; ++IReadBufIndex;
		}
	}

	delete[] IReadBuf;
}

bool FbxLoader::SetCurrentAnimStack(int pIndex)
{
	const int IAnimStackCount = mAnimStackNameArray.GetCount();
	if (!IAnimStackCount || pIndex >= IAnimStackCount) { return false; }
	FbxAnimStack * ICurrentAnimationStack = Scene->FindMember<FbxAnimStack>(mAnimStackNameArray[pIndex]->Buffer());
	if (ICurrentAnimationStack == NULL) { return false; }
	pAnimLayer = ICurrentAnimationStack->GetMember<FbxAnimLayer>();
	Scene->SetCurrentAnimationStack(ICurrentAnimationStack);
	//cout << pAnimLayer->GetName() << endl;

	FbxTakeInfo* ICurrentTakeInfo = Scene->GetTakeInfo(*(mAnimStackNameArray[pIndex]));
	if (ICurrentTakeInfo)
	{
		mStart = ICurrentTakeInfo->mLocalTimeSpan.GetStart();
		mStop = ICurrentTakeInfo->mLocalTimeSpan.GetStop();
	}

	else
	{
		FbxTimeSpan ITimeLineTimeSpan;
		Scene->GetGlobalSettings().GetTimelineDefaultTimeSpan(ITimeLineTimeSpan);
		mStart = ITimeLineTimeSpan.GetStart();
		mStop = ITimeLineTimeSpan.GetStop();
	}

	if (mCache_Start < mStart)
		mStart = mCache_Start;

	if (mCache_Stop > mStop)
		mStop = mCache_Stop;

	mCurrentTime = mStart;
	return true;
}

void FbxLoader::OnTimerClick()
{
	if (mStop > mStart)
	{
		mStatus = MUST_BE_REFRESHED;
		mCurrentTime += FrameTime;

		if (mCurrentTime > mStop)
		{
			mCurrentTime = mStart;
			AnimStop = true;
		}
	}

	else
	{
		mStatus = REFRESHED;
	}
}

void FbxLoader::UpdateVertexPosition(const FbxMesh* pMesh, FbxVector4* pVertices, vector <Vertex::Basic32>& Pos)
{
	int VertexCount = 0;
	int TRIANGLE_VERTEX_COUNT = 3;
	const int IPolygonCount = pMesh->GetPolygonCount();
	const int VERTEX_STRIDE = 4;
	IVertexCount = IPolygonCount * TRIANGLE_VERTEX_COUNT;

	int IVertexCount = 0;
	for (int IPolygonIndex = 0; IPolygonIndex < IPolygonCount; ++IPolygonIndex)
	{
		for (int lVerticeIndex = 0; lVerticeIndex < TRIANGLE_VERTEX_COUNT; ++lVerticeIndex)
		{
			const int lControlPointIndex = pMesh->GetPolygonVertex(IPolygonIndex, lVerticeIndex);
			Pos[IVertexCount].Pos.x = static_cast<float>(pVertices[lControlPointIndex][0]);
			Pos[IVertexCount].Pos.y = static_cast<float>(pVertices[lControlPointIndex][1]);
			Pos[IVertexCount].Pos.z = static_cast<float>(pVertices[lControlPointIndex][2]);
			++IVertexCount;
		}
	}
}

void FbxLoader::FileNameFinder()
{
	if (RootNode)
	{
		for (int i = 0; i < RootNode->GetChildNameCount(); ++i)
		{
			PrintNode(RootNode->GetChild(i));
			printf("���");
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
	//��Ʈ����� �̸��� nodeName�� �����Ѵ�.
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
