#include "GetPosition.h"

FbxAMatrix GetGlobalPosition(FbxNode* pNode, const FbxTime& pTime, FbxPose* pPose, FbxAMatrix* pParentGlobalPosition)
{
	FbxAMatrix IGlobalPosition;
	bool IPositionFound = false;

	if (pPose)
	{
		int INodeIndex = pPose->Find(pNode);

		if (INodeIndex > -1)
		{
			if (pPose->IsBindPose() || !pPose->IsLocalMatrix(INodeIndex))
			{
				IGlobalPosition = GetPoseMatrix(pPose, INodeIndex);
			}

			else
			{
				FbxAMatrix lParentGlobalPosition;

				if (pParentGlobalPosition)
				{
					lParentGlobalPosition = *pParentGlobalPosition;
				}

				else
				{
					if (pNode->GetParent())
					{
						lParentGlobalPosition = GetGlobalPosition(pNode->GetParent(), pTime, pPose);
					}
				}

				FbxAMatrix ILocalPosition = GetPoseMatrix(pPose, INodeIndex);
				IGlobalPosition = lParentGlobalPosition * ILocalPosition;
			}

			IPositionFound = true;
		}
	}

	if (!IPositionFound)
		IGlobalPosition = pNode->EvaluateGlobalTransform(pTime);

	return IGlobalPosition;
}

FbxAMatrix GetPoseMatrix(FbxPose* pPose, int pNodeIndex)
{
	FbxAMatrix IPoseMatrix;
	FbxMatrix IMatrix = pPose->GetMatrix(pNodeIndex);

	return IPoseMatrix;
}

FbxAMatrix GetGeometry(FbxNode* pNode)
{
	const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxAMatrix(lT, lR, lS);
}
