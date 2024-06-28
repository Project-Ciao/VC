// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/TimerWithTick.h"

UTimerWithTick* UTimerWithTick::SetTimerWithTick(const UObject* WorldContextObject, float TimerLength)
{
	UTimerWithTick* BlueprintNode = NewObject<UTimerWithTick>();
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->TimerLength = TimerLength;
	return BlueprintNode;
}

void UTimerWithTick::Activate()
{
	WorldContextObject->GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateUObject(this, &ThisClass::OnTimerEnd), TimerLength, false);
}

void UTimerWithTick::OnTimerEnd()
{
	OnFinish.Broadcast();

	TimerHandle.Invalidate();
	SetReadyToDestroy();
}

void UTimerWithTick::Tick(float DeltaTime)
{
	if (TimerHandle.IsValid())
	{
		const float TimeElapsed = WorldContextObject->GetWorld()->GetTimerManager().GetTimerElapsed(TimerHandle);
		OnTick.Broadcast(TimerLength, TimerLength - TimeElapsed, TimeElapsed, DeltaTime);
	}
}

TStatId UTimerWithTick::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UTimerWithTick, STATGROUP_Tickables);
}

ETickableTickType UTimerWithTick::GetTickableTickType() const
{
	return HasAnyFlags(RF_ClassDefaultObject) ? ETickableTickType::Never : ETickableTickType::Always;
}

bool UTimerWithTick::IsTickable() const
{
	return TimerHandle.IsValid();
}
