// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractiveFoliage/InteractiveFoliageManager.h"

#include "VC.h"
#include "EngineUtils.h"
#include "InstancedFoliageActor.h"
#include "FoliageInstancedStaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "Settings/VCSettings.h"

UE_DISABLE_OPTIMIZATION
AInteractiveFoliageManager::AInteractiveFoliageManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicates = true;
	bAlwaysRelevant = true;
}

void AInteractiveFoliageManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AInteractiveFoliageManager, CutTrees);
}

static TSubclassOf<AInteractiveFoliageManager> GetFoliageManagerClass()
{
	TSubclassOf<AInteractiveFoliageManager> ManagerClass = UVCSettings::Get()->InteractiveFoliageManagerClass;
	if (ManagerClass == nullptr)
	{
		ManagerClass = AInteractiveFoliageManager::StaticClass();
	}
	return ManagerClass;
}

void UInteractiveFoliageManagerSubsystem::PostInitialize()
{
	UWorld* World = Cast<UWorld>(GetOuter());

	if (!World) return;

	if (World->GetNetMode() == ENetMode::NM_Client)
	{
		return;
	}

	AInteractiveFoliageManager* Manager = GetInteractiveFoliageManager(World);

	TSubclassOf<AInteractiveFoliageManager> ManagerClass = GetFoliageManagerClass();

	if (Manager != nullptr && Manager->GetClass() != ManagerClass)
	{
		Manager->Destroy();
		Manager = nullptr;
	}

	if (Manager == nullptr)
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnInfo.Name = ManagerClass->GetFName();
#if WITH_EDITOR
		SpawnInfo.bHideFromSceneOutliner = true;
#endif
		Manager = World->SpawnActor<AInteractiveFoliageManager>(ManagerClass, SpawnInfo);
	}

	if (Manager == nullptr)
	{
		UE_LOG(LogVC, Warning, TEXT("Failed to spawn settings actor in World: $s"), *World->GetName());
	}
}

AInteractiveFoliageManager* UInteractiveFoliageManagerSubsystem::GetInteractiveFoliageManager(const UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return nullptr;

	AInteractiveFoliageManager* Manager = nullptr;

	for (TActorIterator<AInteractiveFoliageManager> It(World, AInteractiveFoliageManager::StaticClass(), EActorIteratorFlags::SkipPendingKill); It; ++It)
	{
		Manager = *It;
		break;
	}

	return Manager;
}

bool AInteractiveFoliageManager::TryCutTree(const FTreeID& Tree)
{
	return CutTree(Tree);
}

void AInteractiveFoliageManager::OnRep_CutTrees(const TArray<FTreeID>& OldCutTrees)
{
	for (const FTreeID& TreeID : CutTrees)
	{
		if (!OldCutTrees.Contains(TreeID))
		{
			TreeID.RemoveInstance();
		}
	}
}

bool AInteractiveFoliageManager::CutTree(const FTreeID& Tree)
{
	if (CutTrees.Contains(Tree))
	{
		return true;
	}
	else
	{
		FTransform InstanceTransform;
		Tree.GetInstanceTransform(InstanceTransform);

		bool RemovedInstance = Tree.RemoveInstance();

		if (RemovedInstance)
		{
			CutTrees.AddUnique(Tree);
			OnTreeCut(Tree, InstanceTransform);
		}

		return RemovedInstance;
	}
}

void AInteractiveFoliageManager::OnTreeCut_Implementation(const FTreeID&, const FTransform&)
{

}

bool FTreeID::RemoveInstance() const
{
	if (IsValid(FoliageComponent))
	{
		return FoliageComponent->RemoveInstance(InstanceID);
	}
	return false;
}

bool FTreeID::GetInstanceTransform(FTransform& OutTransform) const
{
	if (IsValid(FoliageComponent))
	{
		return FoliageComponent->GetInstanceTransform(InstanceID, OutTransform, true);
	}
	return false;
}

UE_ENABLE_OPTIMIZATION