// Fill out your copyright notice in the Description page of Project Settings.


#include "SkinnedFoliageMeshPipeline.h"

#include "Engine/StaticMesh.h"
#include "InterchangeSceneNode.h"
#include "InterchangeMeshNode.h"
#include "InterchangeSourceData.h"
#include "InterchangeStaticMeshFactoryNode.h"
#include "MaterialDomain.h"
#include "Materials/Material.h"
#include "Misc/CString.h"
#include "Nodes/InterchangeBaseNodeContainer.h"
#include "MeshDescription.h"
#include "StaticMeshAttributes.h"
#include "StaticMeshOperations.h"
#include "StaticMeshDescription.h"

UE_DISABLE_OPTIMIZATION
struct FSkinnedVegetationBone
{
	FSkinnedVegetationBone() = default;

	FSkinnedVegetationBone(const FString& InName)
	{
		Name = InName;
	}

	FString Name;
	FString Parent;
	TArray<FString> Children;
	const UInterchangeMeshNode* Mesh;
};

struct FSkinnedVegetationBoneContainer
{
	TArray<FSkinnedVegetationBone> Bones;

	FSkinnedVegetationBone& AddBone(const FString& BoneName)
	{
		return Bones.Emplace_GetRef(FSkinnedVegetationBone(BoneName));
	}

	void SetBoneParent(const FString& BoneName, const FString& ParentName)
	{
		FSkinnedVegetationBone* Bone = FindBone(BoneName);
		FSkinnedVegetationBone* ParentBone = FindBone(ParentName);

		check(Bone != nullptr);
		check(ParentBone != nullptr);

		Bone->Parent = ParentName;
		ParentBone->Children.AddUnique(BoneName);
	}

	int32 GetBoneIndex(const FString& BoneName)
	{
		for (int32 i = 0; i < Bones.Num(); ++i)
		{
			if (Bones[i].Name == BoneName)
			{
				return i;
			}
		}
		return -1;
	}

	FSkinnedVegetationBone* FindBone(const FString& BoneName)
	{
		for (FSkinnedVegetationBone& Bone : Bones)
		{
			if (Bone.Name == BoneName)
			{
				return &Bone;
			}
		}

		return nullptr;
	}

	FSkinnedVegetationBone* FindBone(const UInterchangeMeshNode* Mesh)
	{
		for (FSkinnedVegetationBone& Bone : Bones)
		{
			if (Bone.Mesh == Mesh)
			{
				return &Bone;
			}
		}

		return nullptr;
	}
};

void USkinnedFoliageMeshPipeline::ExecutePipeline(UInterchangeBaseNodeContainer* BaseNodeContainer, const TArray<UInterchangeSourceData*>& InSourceDatas, const FString& ContentBasePath)
{
	if (!BaseNodeContainer || !bCreateFoliageBoneUVs)
	{
		return;
	}

	TSharedPtr<FSkinnedVegetationBoneContainer> BoneContainer = MakeShared<FSkinnedVegetationBoneContainer>();

	TArray<FString> Roots;
	BaseNodeContainer->GetRoots(Roots);
	for (const FString& Root : Roots)
	{
		UE_LOG(LogTemp, Warning, TEXT("Root: %s"), *Root);
		int32 NumChildren = BaseNodeContainer->GetNodeChildrenCount(Root);
		UE_LOG(LogTemp, Warning, TEXT("Num children: %d"), NumChildren);

		for (int i = 0; i < NumChildren; ++i)
		{
			UInterchangeBaseNode* ChildNode = BaseNodeContainer->GetNodeChildren(Root, i);
			UE_LOG(LogTemp, Warning, TEXT("\tRoot Child: %s"), *ChildNode->GetDisplayLabel());

			if (Cast<UInterchangeSceneNode>(ChildNode))
			{
				UE_LOG(LogTemp, Warning, TEXT("\tRoot Child: %s Is scene node!"), *ChildNode->GetDisplayLabel());
			}
		}
	}

	TArray<FString> StaticMeshFactoryNodes;
	BaseNodeContainer->GetNodes(UInterchangeStaticMeshFactoryNode::StaticClass(), StaticMeshFactoryNodes);

	// Even though there are multiple meshes there will only be one static mesh factory node
	if (StaticMeshFactoryNodes.Num() != 1)
	{
		return;
	}
	UInterchangeStaticMeshFactoryNode* StaticMeshFactoryNode = Cast<UInterchangeStaticMeshFactoryNode>(BaseNodeContainer->GetFactoryNode(StaticMeshFactoryNodes[0]));

	BaseNodeContainer->IterateNodesOfType<UInterchangeMeshNode>([&BoneContainer](const FString& NodeUid, UInterchangeMeshNode* SceneNode)
	{
		FString NodeName = SceneNode->GetDisplayLabel();
		FString ParentName = SceneNode->GetParentUid();
		UE_LOG(LogTemp, Warning, TEXT("Mesh Node Uid: %s, Node Name: %s, Parent Name: %s"), *NodeUid, *NodeName, *ParentName);

		TArray<FString> SceneInstances;
		SceneNode->GetSceneInstanceUids(SceneInstances);

		//SceneNode->GetAttribute("UVs", Test);

		for (const FString& SceneInstance : SceneInstances)
		{
			UE_LOG(LogTemp, Warning, TEXT("\tSceneInstance: %s"), *SceneInstance);
			
			BoneContainer->AddBone(SceneInstance).Mesh = SceneNode;
		}
	});
}

void USkinnedFoliageMeshPipeline::ExecutePostFactoryPipeline(const UInterchangeBaseNodeContainer* BaseNodeContainer, const FString& NodeKey, UObject* CreatedAsset, bool bIsAReimport)
{
	UInterchangeStaticMeshFactoryNode* StaticMeshFactoryNode = Cast<UInterchangeStaticMeshFactoryNode>(BaseNodeContainer->GetFactoryNode(NodeKey));
	if (!StaticMeshFactoryNode)
	{
		return;
	}

	UStaticMesh* StaticMesh = Cast<UStaticMesh>(CreatedAsset);
	if (!StaticMesh)
	{
		return;
	}

	FMeshDescription* MeshDescription = StaticMesh->GetMeshDescription(0);
	if (!MeshDescription)
	{
		return;
	}

	FStaticMeshAttributes Attributes(*MeshDescription);

	for (const auto Test : MeshDescription->GetElements())
	{
		UE_LOG(LogTemp, Warning, TEXT("TEst, %s"), *Test.Key.ToString());
	}

	int32 UVChannel = MeshDescription->GetNumUVElementChannels();
	MeshDescription->SuspendUVIndexing();
	TVertexInstanceAttributesRef<FVector2f> UVs = Attributes.GetVertexInstanceUVs();

	TPolygonAttributesRef<FPolygonGroupID> PolygonGroups = Attributes.GetPolygonPolygonGroupIndices();

	int32 LastPatchGroupId = -1;
	for (int32 PolygoneID = 0; PolygoneID < PolygonGroups.GetNumElements(); ++PolygoneID)
	{
		const FPolygonGroupID GroupID = PolygonGroups[PolygoneID];

		UE_LOG(LogTemp, Warning, TEXT("Polygon ID: %d, Group ID: %d"), PolygoneID, GroupID.GetValue());
	}

	// So you have to set the number of UV channels in both places for some reason
	MeshDescription->SetNumUVChannels(UVChannel + 1);
	UVs.SetNumChannels(UVChannel + 1);

	for (const FVertexInstanceID VertexInstanceID : MeshDescription->VertexInstances().GetElementIDs())
	{
		//UVs.Get(VertexInstanceID, UVChannel).X;// = VertexInstanceID.GetValue();
		UVs.Set(VertexInstanceID, UVChannel, FVector2f(VertexInstanceID.GetValue(), 0.f));
	}

	MeshDescription->ResumeUVIndexing();
	StaticMesh->CommitMeshDescription(0);
}
UE_ENABLE_OPTIMIZATION
