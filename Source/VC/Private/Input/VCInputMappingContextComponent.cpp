// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/VCInputMappingContextComponent.h"

#include "EnhancedInputSubsystems.h"
#include "Input/VCInputMappingContextSubsystem.h"

UVCInputMappingContextComponent::UVCInputMappingContextComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;

	BoundPC = nullptr;
}

TArray<FVCInputMap> UVCInputMappingContextComponent::GetInputMapsFromComponents(const TArray<UVCInputMappingContextComponent*>& Components)
{
	TArray<FVCInputMap> InputMaps;
	for (const UVCInputMappingContextComponent* Component : Components)
	{
		if (!IsValid(Component))
		{
			continue;
		}

		for (const FVCInputMap& Map : Component->GetInputMaps())
		{
			InputMaps.AddUnique(Map);
		}
	}

	return InputMaps;
}

void UVCInputMappingContextComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		return;
	}

	if (APawn* PawnOwner = Cast<APawn>(GetOwner()))
	{
		PawnOwner->ReceiveControllerChangedDelegate.AddUniqueDynamic(this, &ThisClass::OnReceiveControllerChanged);
	}
}

void UVCInputMappingContextComponent::OnUnregister()
{
	Super::OnUnregister();

	if (UWorld* World = GetWorld())
	{
		UGameInstance* GameInstance = World->GetGameInstance();

		if (GameInstance)
		{
			for (auto PlayerIt = GameInstance->GetLocalPlayerIterator(); PlayerIt; ++PlayerIt)
			{
				UVCInputMappingContextSubsystem* VCInputMappingSubsystem = ULocalPlayer::GetSubsystem<UVCInputMappingContextSubsystem>(*PlayerIt);
				if (VCInputMappingSubsystem)
				{
					VCInputMappingSubsystem->PopInputContextMappingComponent(this);
				}
			}
		}
	}
}

void UVCInputMappingContextComponent::OnReceiveControllerChanged(APawn* Pawn, AController* OldController, AController* NewController)
{
	check(GetOwner() == Pawn);

	//GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Black, FString::Printf(L"Pawn: %s, Old controller: %s, new controller: %s",
	//	*(Pawn ? Pawn->GetName() : FString()),
	//	*(OldController ? OldController->GetName() : FString()),
	//	*(NewController ? NewController->GetName() : FString())));

	if (APlayerController* PC = Cast<APlayerController>(OldController))
	{
		if (PC == BoundPC)
		{
			if (const ULocalPlayer* LocalPC = PC->GetLocalPlayer())
			{
				UVCInputMappingContextSubsystem* VCInputMappingSubsystem = ULocalPlayer::GetSubsystem<UVCInputMappingContextSubsystem>(LocalPC);
				if (VCInputMappingSubsystem)
				{
					VCInputMappingSubsystem->PopInputContextMappingComponent(this);

					BoundPC = nullptr;
				}
			}
		}
	}

	if (APlayerController* PC = Cast<APlayerController>(NewController))
	{
		if (PC->IsLocalPlayerController())
		{
			if (const ULocalPlayer* LocalPC = PC->GetLocalPlayer())
			{
				UVCInputMappingContextSubsystem* VCInputMappingSubsystem = ULocalPlayer::GetSubsystem<UVCInputMappingContextSubsystem>(LocalPC);
				if (VCInputMappingSubsystem)
				{
					VCInputMappingSubsystem->PushInputContextMappingComponent(this);

					BoundPC = PC;
				}
			}
		}
	}
}

