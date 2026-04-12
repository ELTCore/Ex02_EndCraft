// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EndCraftPlayerController.generated.h"

// 前向声明
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
class UCameraComponent;
class USpringArmComponent;
class AEndCraftHeroCharacter;

// 框选状态
USTRUCT()
struct FSelectionBox
{
	GENERATED_BODY()

	FVector2D StartPos;
	FVector2D EndPos;

	bool IsActive = false;

	FVector2D GetMin() const
	{
		return FVector2D(FMath::Min(StartPos.X, EndPos.X), FMath::Min(StartPos.Y, EndPos.Y));
	}

	FVector2D GetMax() const
	{
		return FVector2D(FMath::Max(StartPos.X, EndPos.X), FMath::Max(StartPos.Y, EndPos.Y));
	}

	bool Contains(const FVector2D& Point) const
	{
		FVector2D Min = GetMin();
		FVector2D Max = GetMax();
		return Point.X >= Min.X && Point.X <= Max.X &&
		       Point.Y >= Min.Y && Point.Y <= Max.Y;
	}
};

/**
 * EndCraft 玩家控制器
 * 处理玩家输入、相机控制和单位选择
 */
UCLASS()
class ENDCRAFT_API AEndCraftPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AEndCraftPlayerController();

	// 获取弹簧臂组件（用于相机缩放）
	USpringArmComponent* GetSpringArmComponent() const;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// 输入绑定
	virtual void SetupInputComponent() override;

	// 相机移动（处理 WASD 和边缘滚动）
	void MoveCamera();

	// 相机缩放
	void ZoomCamera(const FInputActionValue& Value);

	// 单位选择
	void SelectUnits(const FInputActionValue& Value);

	// 右键移动命令
	void IssueMoveCommand();

	// 检查 Shift 键是否按下
	bool IsShiftKeyDown() const;

	// 时停切换
	void ToggleTimeStop(const FInputActionValue& Value);

	// WASD 输入 - 单个方向
	void OnMoveUpStarted(const FInputActionValue& Value);      // W
	void OnMoveUpCompleted(const FInputActionValue& Value);
	void OnMoveDownStarted(const FInputActionValue& Value);    // S
	void OnMoveDownCompleted(const FInputActionValue& Value);
	void OnMoveLeftStarted(const FInputActionValue& Value);    // A
	void OnMoveLeftCompleted(const FInputActionValue& Value);
	void OnMoveRightStarted(const FInputActionValue& Value);   // D
	void OnMoveRightCompleted(const FInputActionValue& Value);

	// 框选开始
	void OnSelectStart(const FInputActionValue& Value);
	void OnSelectUpdate(const FInputActionValue& Value);
	void OnSelectEnd(const FInputActionValue& Value);

private:
	// 计算边缘滚动输入
	FVector2D CalculateEdgeScrollInput() const;

	// 将屏幕边缘位置转换为滚动强度
	float GetEdgeScrollIntensity(float Position, float EdgeThickness) const;

	// 执行框选
	void PerformBoxSelection();

	// 执行单选
	void PerformSingleSelection();

	// 清除所有选中单位
	void ClearAllSelections();

	// 射线检测获取单位
	AEndCraftHeroCharacter* GetHeroUnderCursor() const;

	// 获取鼠标在场景中的位置
	bool GetMouseWorldLocation(FVector& OutLocation) const;

private:
	// 输入映射上下文
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	// 输入动作 - WASD
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_MoveUp;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_MoveDown;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_MoveLeft;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_MoveRight;

	// 输入动作 - 其他
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_ZoomCamera;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_SelectUnits;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_TimeStop;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_MoveCommand;

	// 边缘滚动设置
	UPROPERTY(EditAnywhere, Category = "Camera|EdgeScroll")
	float EdgeScrollThickness;

	UPROPERTY(EditAnywhere, Category = "Camera|EdgeScroll")
	float EdgeScrollSpeed;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float CameraMoveSpeed;

	// WASD 移动输入累积（Y 轴：W/S, X 轴：A/D）
	float MoveInputY;  // W(正) / S(负)
	float MoveInputX;  // D(正) / A(负)

	// 框选状态
	FSelectionBox SelectionBox;

	// 框选 UI（可选）
	UPROPERTY()
	UUserWidget* SelectionBoxWidget;
};
