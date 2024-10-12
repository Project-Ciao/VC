// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/VCInputMappingContextComponent.h"

#include "EnhancedInputSubsystems.h"
#include "Input/VCInputMappingContextSubsystem.h"

UVCInputMappingContextComponent::UVCInputMappingContextComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;

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

bool UVCInputMappingContextComponent::PushInputMappingContext(APlayerController* Player)
{
	if (Player == nullptr)
	{
		return false;
	}

	if (BoundPC == nullptr)
	{
		if (!Player->IsLocalPlayerController())
		{
			return false;
		}

		if (const ULocalPlayer* LocalPC = Player->GetLocalPlayer())
		{
			UVCInputMappingContextSubsystem* VCInputMappingSubsystem = ULocalPlayer::GetSubsystem<UVCInputMappingContextSubsystem>(LocalPC);
			if (VCInputMappingSubsystem)
			{
				const bool PushedInputMappingComponent = VCInputMappingSubsystem->PushInputContextMappingComponent(this);
				if (PushedInputMappingComponent)
				{
					BoundPC = Player;
					BP_OnInputMappingContextPushed(BoundPC);
					OnInputMappingContextPushed.Broadcast(this, BoundPC);
					return true;
				}
			}
		}
	}

	return false;
}

bool UVCInputMappingContextComponent::PopInputMappingContext()
{
	if (BoundPC != nullptr)
	{
		if (const ULocalPlayer* LocalPC = BoundPC->GetLocalPlayer())
		{
			UVCInputMappingContextSubsystem* VCInputMappingSubsystem = ULocalPlayer::GetSubsystem<UVCInputMappingContextSubsystem>(LocalPC);
			if (VCInputMappingSubsystem)
			{
				const bool PoppedInputMappingComponent = VCInputMappingSubsystem->PopInputContextMappingComponent(this);
				if (PoppedInputMappingComponent)
				{
					BP_OnInputMappingContextPopped(BoundPC);
					OnInputMappingContextPopped.Broadcast(this, BoundPC);
					BoundPC = nullptr;
					return true;
				}
			}
		}
	}

	return false;
}

void UVCInputMappingContextComponent::ListenForPawnControllerChange(APawn* Pawn)
{
	if (Pawn != nullptr)
	{
		Pawn->ReceiveControllerChangedDelegate.AddUniqueDynamic(this, &ThisClass::OnReceiveControllerChanged);
	}
}

void UVCInputMappingContextComponent::UnlistenToPawnControllerChange(APawn* Pawn)
{
	if (Pawn != nullptr)
	{
		Pawn->ReceiveControllerChangedDelegate.RemoveAll(this);
	}
}

APawn* UVCInputMappingContextComponent::GetInputPawn_Implementation()
{
	if (APawn* PawnOwner = Cast<APawn>(GetOwner()))
	{
		return PawnOwner;
	}
	return nullptr;
}

void UVCInputMappingContextComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		return;
	}

	if (APawn* PawnOwner = GetInputPawn())
	{
		ListenForPawnControllerChange(PawnOwner);

		if (APlayerController* PC = Cast<APlayerController>(PawnOwner->GetController()))
		{
			if (PC->IsLocalPlayerController())
			{
				PushInputMappingContext(PC);
			}
		}
	}
}

void UVCInputMappingContextComponent::OnUnregister()
{
	Super::OnUnregister();

	PopInputMappingContext();
}

void UVCInputMappingContextComponent::OnReceiveControllerChanged(APawn* Pawn, AController* OldController, AController* NewController)
{
	//check(GetOwner() == Pawn);

	//GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Black, FString::Printf(L"Pawn: %s, Old controller: %s, new controller: %s",
	//	*(Pawn ? Pawn->GetName() : FString()),
	//	*(OldController ? OldController->GetName() : FString()),
	//	*(NewController ? NewController->GetName() : FString())));

	if (APlayerController* PC = Cast<APlayerController>(OldController))
	{
		if (PC->IsLocalPlayerController())
		{
			if (PC == BoundPC)
			{
				PopInputMappingContext();
			}
		}
	}

	if (APlayerController* PC = Cast<APlayerController>(NewController))
	{
		if (PC->IsLocalPlayerController())
		{
			PushInputMappingContext(PC);
		}
	}
}

