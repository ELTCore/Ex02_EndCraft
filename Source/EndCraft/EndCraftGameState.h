// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "EndCraftGameState.generated.h"

/**
 * EndCraft 游戏状态
 * 管理游戏全局状态（时停状态、选中单位等）
 */
UCLASS()
class ENDCRAFT_API AEndCraftGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AEndCraftGameState();

protected:
	virtual void BeginPlay() override;

public:
	// 时停系统
	UFUNCTION(BlueprintCallable, Category = "TimeStop")
	void EnterTimeStop();

	UFUNCTION(BlueprintCallable, Category = "TimeStop")
	void ExitTimeStop();

	UFUNCTION(BlueprintPure, Category = "TimeStop")
	bool IsTimeStopActive() const { return bTimeStopActive; }

	// 单位管理
	UFUNCTION(BlueprintCallable, Category = "Selection")
	void AddSelectedUnit(AActor* Unit);

	UFUNCTION(BlueprintCallable, Category = "Selection")
	void ClearSelectedUnits();

	UFUNCTION(BlueprintPure, Category = "Selection")
	TArray<AActor*> GetSelectedUnits() const { return SelectedUnits; }

	// 游戏阶段
	UFUNCTION(BlueprintPure, Category = "Game")
	bool IsGamePaused() const { return bTimeStopActive; }

protected:
	// 时停状态
	UPROPERTY(BlueprintReadOnly, Category = "TimeStop")
	bool bTimeStopActive;

	// 选中的单位列表
	UPROPERTY(BlueprintReadOnly, Category = "Selection")
	TArray<AActor*> SelectedUnits;

	// 时间膨胀倍数（时停时为 0）
	UPROPERTY(EditAnywhere, Category = "TimeStop")
	float TimeDilationFactor;
};
