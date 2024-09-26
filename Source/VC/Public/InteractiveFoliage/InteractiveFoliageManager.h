// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "Subsystems/WorldSubsystem.h"
#include "InteractiveFoliageManager.generated.h"

class UFoliageInstancedStaticMeshComponent;

USTRUCT(BlueprintType)
struct FTreeID
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interactive Foliage|Tree ID")
	UFoliageInstancedStaticMeshComponent* FoliageComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interactive Foliage|Tree ID")
	int32 InstanceID;

	FORCEINLINE bool operator==(const FTreeID& Other) const
	{
		return Other.FoliageComponent == FoliageComponent && Other.InstanceID == InstanceID;
	}

	bool RemoveInstance() const;
	bool GetInstanceTransform(FTransform& OutTransform) const;
};


/**
 * 
 */
UCLASS(NotPlaceable, Blueprintable)
class VC_API AInteractiveFoliageManager : public AInfo
{
	GENERATED_BODY()
	
public:
	AInteractiveFoliageManager(const FObjectInitializer& ObjectInitializer);
	
	UFUNCTION(BlueprintCallable, Category = "Interactive Foliage|Manager")
	bool TryCutTree(const FTreeID& Tree);
	
protected:
	UPROPERTY(ReplicatedUsing=OnRep_CutTrees)
	TArray<FTreeID> CutTrees;

	UFUNCTION()
	void OnRep_CutTrees(const TArray<FTreeID>& OldCutTrees);

	bool CutTree(const FTreeID& Tree);

	UFUNCTION(BlueprintNativeEvent, Category = "Interactive Foliage|Manager")
	void OnTreeCut(const FTreeID& Tree, const FTransform& InstanceTransform);
};

UCLASS()
class VC_API UInteractiveFoliageManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	virtual void PostInitialize() override;

	UFUNCTION(BlueprintPure, Category = "Interactive Foliage")
	static AInteractiveFoliageManager* GetInteractiveFoliageManager(const UObject* WorldContextObject);
};