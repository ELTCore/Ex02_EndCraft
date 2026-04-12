// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EndCraftCameraPawn.generated.h"

/**
 * EndCraft 相机 Pawn
 * 用于 RTS 风格的相机控制
 */
UCLASS()
class ENDCRAFT_API AEndCraftCameraPawn : public APawn
{
	GENERATED_BODY()

public:
	AEndCraftCameraPawn();

	// 获取弹簧臂组件
	UFUNCTION(BlueprintPure, Category = "Camera")
	USpringArmComponent* GetSpringArmComponent() const { return SpringArmComponent; }

	// 获取相机位置
	UFUNCTION(BlueprintPure, Category = "Camera")
	UCameraComponent* GetCameraComponent() const { return CameraComponent; }

	// 相机移动
	void AddCameraMovement(const FVector& Input, float Speed);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// 相机
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UCameraComponent* CameraComponent;
	
	// 弹簧臂组件
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USpringArmComponent* SpringArmComponent;
};
