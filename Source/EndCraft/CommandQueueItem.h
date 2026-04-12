// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommandQueueItem.generated.h"

/**
 * 指令类型
 */
UENUM(BlueprintType)
enum class ECommandType : uint8
{
	Move       UMETA(DisplayName = "移动"),
	Attack     UMETA(DisplayName = "攻击"),
	Skill      UMETA(DisplayName = "技能"),
	Wait       UMETA(DisplayName = "等待")
};

/**
 * 指令队列项目
 */
USTRUCT(BlueprintType)
struct FCommandQueueItem
{
	GENERATED_BODY()

	// 指令类型
	UPROPERTY(BlueprintReadOnly, Category = "Command")
	ECommandType CommandType;

	// 目标位置
	UPROPERTY(BlueprintReadOnly, Category = "Command")
	FVector TargetLocation;

	// 目标 Actor（攻击目标）
	UPROPERTY(BlueprintReadOnly, Category = "Command")
	TWeakObjectPtr<AActor> TargetActor;

	// 技能 ID（如果是技能指令）
	UPROPERTY(BlueprintReadOnly, Category = "Command")
	int32 SkillID;

	// 等待时间（如果是等待指令）
	UPROPERTY(BlueprintReadOnly, Category = "Command")
	float WaitTime;

	// 指令创建时间
	UPROPERTY(BlueprintReadOnly, Category = "Command")
	float CreationTime;

	FCommandQueueItem()
		: CommandType(ECommandType::Move)
		, TargetLocation(FVector::ZeroVector)
		, TargetActor(nullptr)
		, SkillID(0)
		, WaitTime(0.0f)
		, CreationTime(0.0f)
	{
	}

	// 创建移动指令
	static FCommandQueueItem CreateMoveCommand(const FVector& Location)
	{
		FCommandQueueItem Item;
		Item.CommandType = ECommandType::Move;
		Item.TargetLocation = Location;
		return Item;
	}

	// 创建攻击指令
	static FCommandQueueItem CreateAttackCommand(AActor* Target)
	{
		FCommandQueueItem Item;
		Item.CommandType = ECommandType::Attack;
		Item.TargetActor = Target;
		return Item;
	}

	// 创建等待指令
	static FCommandQueueItem CreateWaitCommand(float Duration)
	{
		FCommandQueueItem Item;
		Item.CommandType = ECommandType::Wait;
		Item.WaitTime = Duration;
		return Item;
	}

	bool IsValid() const
	{
		switch (CommandType)
		{
			case ECommandType::Move:
				return !TargetLocation.IsZero();
			case ECommandType::Attack:
				return TargetActor.IsValid();
			case ECommandType::Wait:
				return WaitTime > 0.0f;
			default:
				return false;
		}
	}
};
