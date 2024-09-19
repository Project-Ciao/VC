// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_Footstep.h"

#include "Kismet/KismetSystemLibrary.h"
#include "NativeGameplayTags.h"
#include "Animation/FootstepMaterialInterface.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(Tag_Footstep_Default, "VC.Animation.Notify.FootstepType.Step");
UE_DEFINE_GAMEPLAY_TAG_STATIC(Tag_Foot_Default, "VC.Animation.Notify.Foot.Left");

UAnimNotify_Footstep::UAnimNotify_Footstep(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FootSocketName = FName("Base-HumanLFoot");
	FootstepType = Tag_Footstep_Default;
	Foot = Tag_Foot_Default;
}

void UAnimNotify_Footstep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp)
	{
		return;
	}

	AActor* MeshOwner = MeshComp->GetOwner();
	if (!MeshOwner)
	{
		return;
	}

	UWorld* World = MeshComp->GetWorld();
	check(World);

	const FVector FootLocation = MeshComp->GetSocketLocation(FootSocketName);
	const FRotator FootRotation = MeshComp->GetSocketRotation(FootSocketName);
	const FVector TraceEnd = FootLocation - MeshOwner->GetActorUpVector() * TraceLength;

	FMaterialFootstep Footstep;
	Footstep.Animation = Animation;
	Footstep.MeshComp = MeshComp;
	Footstep.FootstepType = FootstepType;
	Footstep.Foot = Foot;

	if (UKismetSystemLibrary::LineTraceSingle(MeshOwner /*used by bIgnoreSelf*/, FootLocation, TraceEnd, TraceChannel, true /*bTraceComplex*/, MeshOwner->Children,
		DrawDebugType, Footstep.Hit, true /*bIgnoreSelf*/))
	{
		if (!Footstep.Hit.PhysMaterial.Get())
		{
			return;
		}

		if (UKismetSystemLibrary::DoesImplementInterface(Footstep.Hit.PhysMaterial.Get(), UFootstepMaterialInterface::StaticClass()))
		{
			IFootstepMaterialInterface::Execute_OnFootstep(Footstep.Hit.PhysMaterial.Get(), Footstep);
		}
	}
}

FString UAnimNotify_Footstep::GetNotifyName_Implementation() const
{
	return FString::Printf(TEXT("Footstep Type: %s\nFoot: %s"), *FootstepType.ToString(), *Foot.ToString());
}

