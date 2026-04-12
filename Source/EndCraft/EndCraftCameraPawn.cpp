// Copyright Epic Games, Inc. All Rights Reserved.

#include "EndCraftCameraPawn.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

AEndCraftCameraPawn::AEndCraftCameraPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	// 创建弹簧臂组件
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	SpringArmComponent->TargetArmLength = 1500.0f;
	SpringArmComponent->bEnableCameraRotationLag = false;
	SpringArmComponent->bUsePawnControlRotation = false;  // 不使用 Control Rotation，使用自定义旋转
	SpringArmComponent->SetRelativeRotation(FRotator(-60.0f, 0.0f, 0.0f));  // 俯视角度
	RootComponent = SpringArmComponent;

	// 创建相机组件
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;
}

void AEndCraftCameraPawn::BeginPlay()
{
	Super::BeginPlay();
}

void AEndCraftCameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEndCraftCameraPawn::AddCameraMovement(const FVector& Input, float Speed)
{
	if (Input.IsNearlyZero())
	{
		return;
	}

	// 获取当前 Pawn 的朝向（只使用 Yaw）
	FRotator PawnRotation = GetActorRotation();
	PawnRotation.Pitch = 0.0f;
	PawnRotation.Roll = 0.0f;

	// 计算移动方向
	const FVector Forward = PawnRotation.RotateVector(FVector::ForwardVector);
	const FVector Right = PawnRotation.RotateVector(FVector::RightVector);

	// 计算移动向量（Y 轴负向是向前，X 轴正向是向右）
	FVector MoveDirection = Forward * Input.Y + Right * Input.X;
	MoveDirection.Z = 0.0f;  // 保持水平移动

	// 直接设置新位置
	FVector NewLocation = GetActorLocation() + MoveDirection * Speed * GetWorld()->GetDeltaSeconds();
	SetActorLocation(NewLocation);
}
