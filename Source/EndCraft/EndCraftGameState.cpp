// Copyright Epic Games, Inc. All Rights Reserved.

#include "EndCraftGameState.h"
#include "Kismet/GameplayStatics.h"

AEndCraftGameState::AEndCraftGameState()
{
	bTimeStopActive = false;
	TimeDilationFactor = 0.0f;
}

void AEndCraftGameState::BeginPlay()
{
	Super::BeginPlay();
}

void AEndCraftGameState::EnterTimeStop()
{
	if (!bTimeStopActive)
	{
		bTimeStopActive = true;
		
		// 设置全局时间膨胀
		UGameplayStatics::SetGlobalTimeDilation(this, TimeDilationFactor);
		
		UE_LOG(LogTemp, Log, TEXT("Time Stop Activated"));
	}
}

void AEndCraftGameState::ExitTimeStop()
{
	if (bTimeStopActive)
	{
		bTimeStopActive = false;
		
		// 恢复全局时间膨胀
		UGameplayStatics::SetGlobalTimeDilation(this, 1.0f);
		
		UE_LOG(LogTemp, Log, TEXT("Time Stop Deactivated"));
	}
}

void AEndCraftGameState::AddSelectedUnit(AActor* Unit)
{
	if (Unit && !SelectedUnits.Contains(Unit))
	{
		SelectedUnits.Add(Unit);
	}
}

void AEndCraftGameState::ClearSelectedUnits()
{
	// 清除前先通知单位取消选中状态
	for (AActor* Unit : SelectedUnits)
	{
		if (Unit)
		{
			// 后续会通过接口通知单位
		}
	}
	SelectedUnits.Empty();
}
