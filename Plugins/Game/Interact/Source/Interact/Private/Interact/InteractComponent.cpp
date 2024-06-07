// Fill out your copyright notice in the Description page of Project Settings.


#include "Interact/InteractComponent.h"

#include "Interact/InteractableComponent.h"

// Sets default values for this component's properties
UInteractComponent::UInteractComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

	TraceLength = 256.f;
	TraceChannel = ECC_WorldDynamic;
	bDebugTrace = false;
}

UPrimitiveComponent* UInteractComponent::GetOwnerOverlapComponent_Implementation()
{
	return Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());
}

void UInteractComponent::InputBeginUse()
{
	if (CurrentInteractableComponents.Num() >= 1)
	{
		BeginUse(CurrentInteractableComponents[0]);
		InteractedComponent = CurrentInteractableComponents[0];
	}
}

void UInteractComponent::InputEndUse()
{
	if (IsValid(InteractedComponent))
	{
		EndUse(InteractedComponent);
		InteractedComponent = nullptr;
	}
}

void UInteractComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TArray<UInteractableComponent*> OldInteractableComponents = CurrentInteractableComponents;
	UInteractableComponent* OldInteractableComponent = CurrentInteractableComponents.IsValidIndex(0) ? CurrentInteractableComponents[0] : nullptr;
	OldInteractableComponent = IsValid(OldInteractableComponent) ? OldInteractableComponent : nullptr;
	
	switch (GetTraceMethod())
	{
	case EInteractComponentTraceMethod::Overlap:
		CurrentInteractableComponents = GetOverlappingInteractableComponents();
		break;

	case EInteractComponentTraceMethod::Trace:
	{
		CurrentInteractableComponents.Empty();
		UInteractableComponent* HitComponent = TraceInteractableComponents();
		if (HitComponent)
		{
			CurrentInteractableComponents.Add(HitComponent);
		}
		break;
	}

	default:
		CurrentInteractableComponents.Empty();
		break;
	}

	// Compare the old items with the new items, fire the delegate if it's changed
	if (OldInteractableComponents.Num() != CurrentInteractableComponents.Num())
	{
		OnCurrentInteractableComponentsChange.Broadcast(CurrentInteractableComponents);
	}
	else
	{
		for (int i = 0; i < CurrentInteractableComponents.Num(); ++i)
		{
			if (!OldInteractableComponents.Contains(CurrentInteractableComponents[i]))
			{
				OnCurrentInteractableComponentsChange.Broadcast(CurrentInteractableComponents);
				break;
			}
		}
	}

	UInteractableComponent* CurrentInteractableComponent = CurrentInteractableComponents.IsValidIndex(0) ? CurrentInteractableComponents[0] : nullptr;
	if (OldInteractableComponent != CurrentInteractableComponent)
	{
		CurrentInteractableComponentChange(OldInteractableComponent, CurrentInteractableComponent);
	}
}

TArray<UInteractableComponent*> UInteractComponent::GetOverlappingInteractableComponents()
{
	TArray<UInteractableComponent*> Out;

	if (GetOwnerOverlapComponent())
	{
		TArray<AActor*> OverlappedActors;
		GetOwnerOverlapComponent()->GetOverlappingActors(OverlappedActors);

		// Look at all overlapping actors, find their interact components
		for (AActor* Actor : OverlappedActors)
		{
			TArray<UInteractableComponent*> InteractableComponents;
			Actor->GetComponents<UInteractableComponent*>(InteractableComponents);

			for (UInteractableComponent* InteractableComponent : InteractableComponents)
			{
				// If they have any interact triggers listed, then check overlap against those rather than the entire actor
				TArray<UPrimitiveComponent*> InteractTriggers = InteractableComponent->GetInteractTriggers();

				if (InteractTriggers.Num() > 0)
				{
					for (UPrimitiveComponent* InteractTrigger : InteractTriggers)
					{
						if (GetOwnerOverlapComponent()->IsOverlappingComponent(InteractTrigger) && InteractableComponent->IsUseable(this))
						{
							Out.Add(InteractableComponent);
							break;
						}
					}
				}
				else
				{
					if (InteractableComponent->IsUseable(this))
					{
						Out.Add(InteractableComponent);
					}
				}
			}
		}
	}

	return Out;
}

UInteractableComponent* UInteractComponent::TraceInteractableComponents()
{
	if (APawn* PawnOwner = Cast<APawn>(GetOwner()))
	{
		if (APlayerController* Controller = Cast<APlayerController>(PawnOwner->GetController()))
		{
			if (APlayerCameraManager* CameraManager = Controller->PlayerCameraManager)
			{
				const FVector TraceStart = CameraManager->GetCameraLocation();
				const FVector TraceEnd = TraceStart + (CameraManager->GetCameraRotation().Vector() * TraceLength);

				FCollisionQueryParams CollisionQueryParams;
				CollisionQueryParams.AddIgnoredActor(PawnOwner);
				CollisionQueryParams.bDebugQuery = bDebugTrace;

				TArray<FHitResult> Hits;
				GetWorld()->LineTraceMultiByChannel(Hits, TraceStart, TraceEnd, TraceChannel, CollisionQueryParams);

				for (const FHitResult& Hit : Hits)
				{
					if (AActor* Actor = Hit.GetActor())
					{
						TArray<UInteractableComponent*> InteractableComponents;
						Actor->GetComponents<UInteractableComponent*>(InteractableComponents);

						for (UInteractableComponent* InteractableComponent : InteractableComponents)
						{
							// If they have any interact triggers listed, then check trace against those rather than the entire actor
							TArray<UPrimitiveComponent*> InteractTriggers = InteractableComponent->GetInteractTriggers();

							if (InteractTriggers.Num() > 0)
							{
								for (UPrimitiveComponent* InteractTrigger : InteractTriggers)
								{
									if (Hit.GetComponent() == InteractTrigger && InteractableComponent->IsUseable(this))
									{
										if (bDebugTrace)
										{
											DrawDebugLine(
												GetWorld(),
												TraceStart,
												Hit.Location,
												FColor(0, 255, 0),
												false, -1, 0
											);
										}
										return InteractableComponent;
									}
								}
							}
							else
							{
								if (InteractableComponent->IsUseable(this))
								{
									if (bDebugTrace)
									{
										DrawDebugLine(
											GetWorld(),
											TraceStart,
											Hit.Location,
											FColor(0, 255, 0),
											false, -1, 0
										);
									}
									return InteractableComponent;
								}
							}
						}
					}
				}

				if (bDebugTrace)
				{
					DrawDebugLine(
						GetWorld(),
						TraceStart,
						TraceEnd,
						FColor(255, 0, 0),
						false, -1, 0
					);
				}
			}
		}
	}
	return nullptr;
}

EInteractComponentTraceMethod UInteractComponent::GetTraceMethod_Implementation()
{
	if (GetInteractComponentTraceMethodDelegate.IsBound())
	{
		return GetInteractComponentTraceMethodDelegate.Execute(this);
	}
	else
	{
		return TraceMethod;
	}
}

bool UInteractComponent::IsLocallyControlled() const
{
	const APawn* PawnOwner = Cast<APawn>(GetOwner());
	return PawnOwner ? PawnOwner->IsLocallyControlled() : false;
}

void UInteractComponent::BeginUse(UInteractableComponent* InteractableComponent)
{
	switch (InteractableComponent->GetReplicationPolicy())
	{
	case EUseReplicationPolicy::RunLocally:
		InteractableComponent->BeginUse(this);
		break;

	case EUseReplicationPolicy::Server:
		if (GetOwner()->HasAuthority())
		{
			InteractableComponent->BeginUse(this);
		}
		else
		{
			Server_BeginUse(InteractableComponent);
		}
		break;

	case EUseReplicationPolicy::Both:
		if (GetOwner()->HasAuthority())
		{
			InteractableComponent->BeginUse(this);
		}
		else
		{
			InteractableComponent->BeginUse(this);
			Server_BeginUse(InteractableComponent);
		}
		break;

	default:
		break;
	}
}

void UInteractComponent::EndUse(UInteractableComponent* InteractableComponent)
{
	switch (InteractableComponent->GetReplicationPolicy())
	{
	case EUseReplicationPolicy::RunLocally:
		InteractableComponent->EndUse(this);
		break;

	case EUseReplicationPolicy::Server:
		if (GetOwner()->HasAuthority())
		{
			InteractableComponent->EndUse(this);
		}
		else
		{
			Server_EndUse(InteractableComponent);
		}
	default:
		break;
	}
}

void UInteractComponent::CurrentInteractableComponentChange(UInteractableComponent* OldInteractableComponent, UInteractableComponent* NewInteractableComponent)
{
	if (APawn* PawnOwner = Cast<APawn>(GetOwner()))
	{
		// only run this function on locally, player controlled pawns
		if (!(PawnOwner->IsPlayerControlled() && PawnOwner->IsLocallyControlled()))
		{
			return;
		}
	}

	if (OldInteractableComponent)
	{
		OldInteractableComponent->EndHighlight(this);
	}

	if (NewInteractableComponent)
	{
		NewInteractableComponent->BeginHighlight(this);
	}

	OnCurrentInteractableComponentChange.Broadcast(OldInteractableComponent, NewInteractableComponent);
}

bool UInteractComponent::Server_BeginUse_Validate(UInteractableComponent* InteractableComponent)
{
	return true;
}

void UInteractComponent::Server_BeginUse_Implementation(UInteractableComponent* InteractableComponent)
{
	if (InteractableComponent)
	{
		InteractableComponent->BeginUse(this);
	}
}

bool UInteractComponent::Server_EndUse_Validate(UInteractableComponent* InteractableComponent)
{
	return true;
}

void UInteractComponent::Server_EndUse_Implementation(UInteractableComponent* InteractableComponent)
{
	if (InteractableComponent)
	{
		InteractableComponent->EndUse(this);
	}
}
