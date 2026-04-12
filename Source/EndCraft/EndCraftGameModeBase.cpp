// Copyright Epic Games, Inc. All Rights Reserved.

#include "EndCraftGameModeBase.h"
#include "Engine/World.h"

AEndCraftGameModeBase::AEndCraftGameModeBase()
{
	// 在这里设置默认的 Pawn 类
	// 后续会在蓝图中配置
}

void AEndCraftGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	// 游戏开始时的初始化逻辑
	UE_LOG(LogTemp, Log, TEXT("EndCraft GameMode Started"));
}
