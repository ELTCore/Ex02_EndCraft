// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EndCraftGameModeBase.generated.h"

/**
 * EndCraft 游戏模式基类
 * 定义游戏的基本规则和模式
 */
UCLASS()
class ENDCRAFT_API AEndCraftGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AEndCraftGameModeBase();

protected:
	// 游戏开始时的初始化
	virtual void BeginPlay() override;
};
