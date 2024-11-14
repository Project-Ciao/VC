// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DynamicLevelComponent.generated.h"

class ULevelStreamingDynamic;

USTRUCT(BlueprintType)
struct VC_API FDynamicLevelOptions
{
	GENERATED_BODY()

	FDynamicLevelOptions()
	{
		LevelGUID.Invalidate();
		Level = nullptr;
		OptionalLevelStreamingClass = nullptr;
		LevelTransform = FTransform::Identity;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dynamic Level Options")
	FGuid LevelGUID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dynamic Level Options")
	TSoftObjectPtr<UWorld> Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dynamic Level Options")
	TSubclassOf<ULevelStreamingDynamic> OptionalLevelStreamingClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dynamic Level Options")
	FTransform LevelTransform;

	bool IsValid() const;
	FString GetDynamicLevelName() const;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VC_API UDynamicLevelComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDynamicLevelComponent();

	FDynamicLevelOptions GetLevelOptions() const { return LevelOptions; }

	bool AreLevelOptionsValid() const { return LevelOptions.IsValid(); }

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, CallInEditor, Category = "Dynamic Level Component")
	void GenerateNewLevelGUID();

	UFUNCTION(BlueprintCallable, Category = "Dynamic Level Component")
	bool SpawnLevel(bool MulticastSpawn);

protected:
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SpawnLevel(const FDynamicLevelOptions& InLevelOptions);

	virtual void BeginPlay() override;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Dynamic Level Component", meta = (ExposeOnSpawn = true))
	FDynamicLevelOptions LevelOptions;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Dynamic Level Component")
	ULevelStreamingDynamic* LevelStreaming;
};
