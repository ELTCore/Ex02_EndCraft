// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CommandQueueItem.h"
#include "EndCraftHeroCharacter.generated.h"

/**
 * EndCraft 英雄单位基类
 */
UCLASS()
class ENDCRAFT_API AEndCraftHeroCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AEndCraftHeroCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	// 选中/取消选中
	UFUNCTION(BlueprintCallable, Category = "Selection")
	void SetSelected(bool bInSelected);

	UFUNCTION(BlueprintPure, Category = "Selection")
	bool IsSelected() const { return bIsSelected; }

	// 指令系统 - 添加指令到队列
	UFUNCTION(BlueprintCallable, Category = "Command")
	void AddCommand(const FCommandQueueItem& Command);

	// 清除指令队列
	UFUNCTION(BlueprintCallable, Category = "Command")
	void ClearCommandQueue();

	// 获取队列长度
	UFUNCTION(BlueprintPure, Category = "Command")
	int32 GetCommandQueueLength() const { return CommandQueue.Num(); }

	// 获取单位位置
	UFUNCTION(BlueprintPure, Category = "Hero")
	FVector GetHeroLocation() const { return GetActorLocation(); }

protected:
	// 选中指示器（光圈）
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* SelectionCircleComponent;

	// 选中状态
	UPROPERTY(BlueprintReadOnly, Category = "Selection")
	bool bIsSelected;

	// 指令队列
	UPROPERTY(BlueprintReadOnly, Category = "Command")
	TArray<FCommandQueueItem> CommandQueue;

	// 当前正在执行的指令
	UPROPERTY()
	FCommandQueueItem CurrentCommand;

	// 是否正在执行指令
	UPROPERTY()
	bool bIsExecutingCommand;

	// 指令执行计时
	UPROPERTY()
	float CommandTimer;

protected:
	/** Cast reference to the AI Controlling this unit */
	TObjectPtr<class AAIController> AIController;

private:
	// 处理指令队列
	void ProcessCommandQueue(float DeltaTime);

	// 执行单个指令
	void ExecuteCommand(const FCommandQueueItem& Command);

	// 完成当前指令
	void CompleteCurrentCommand();
};
