// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_Footstep.h"

#include "Kismet/KismetSystemLibrary.h"
#include "NativeGameplayTags.h"
#include "Animation/FootstepMaterialInterface.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(Tag_Footstep_Default, "VC.Animation.Notify.FootstepType.Step");
UE_DEFINE_GAMEPLAY_TAG_STATIC(Tag_Foot_Default, "VC.Animation.Notify.Foot.Left");

static TAutoConsoleVariable<bool> CVarDebugShowFootstepTraces(TEXT("VC.Animation.DebugFootstepTraces"),
	false,
	TEXT("Show AnimNotify footstep traces"),
	ECVF_Cheat);

UAnimNotify_Footstep::UAnimNotify_Footstep(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FootSocketName = FName("Base-HumanLFoot");
	FootstepType = Tag_Footstep_Default;
	Foot = Tag_Foot_Default;

	TraceLength = 50.f;
	TraceHalfSize = FVector(4.f);
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

	FMaterialFootstep Footstep;
	Footstep.Animation = Animation;
	Footstep.MeshComp = MeshComp;
	Footstep.FootstepType = FootstepType;
	Footstep.Foot = Foot;

	if (TraceFootstep(Footstep))
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

bool UAnimNotify_Footstep::TraceFootstep(FMaterialFootstep& Footstep)
{
	AActor* MeshOwner = Footstep.MeshComp->GetOwner();
	if (!MeshOwner)
	{
		return false;
	}

	const FVector FootLocation = Footstep.MeshComp->GetSocketLocation(FootSocketName);
	const FRotator FootRotation = Footstep.MeshComp->GetComponentRotation(); //Footstep.MeshComp->GetSocketRotation(FootSocketName);
	const FVector TraceEnd = FootLocation - MeshOwner->GetActorUpVector() * TraceLength;

	const EDrawDebugTrace::Type FootstepDrawDebugType = CVarDebugShowFootstepTraces.GetValueOnGameThread() ? EDrawDebugTrace::ForDuration : DrawDebugType.GetValue();

	return UKismetSystemLibrary::BoxTraceSingle(MeshOwner, FootLocation, TraceEnd, TraceHalfSize, FootRotation, TraceChannel, true, MeshOwner->Children, FootstepDrawDebugType, Footstep.Hit, true);

	//return UKismetSystemLibrary::LineTraceSingle(MeshOwner /*used by bIgnoreSelf*/, FootLocation, TraceEnd, TraceChannel, true /*bTraceComplex*/, MeshOwner->Children,
	//	DrawDebugType, Footstep.Hit, true /*bIgnoreSelf*/);
}

