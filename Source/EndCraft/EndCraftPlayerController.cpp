// Copyright Epic Games, Inc. All Rights Reserved.

#include "EndCraftPlayerController.h"
#include "EndCraftGameState.h"
#include "EndCraftHeroCharacter.h"
#include "EndCraftCameraPawn.h"
#include "CommandQueueItem.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

AEndCraftPlayerController::AEndCraftPlayerController()
{
	// 启用鼠标光标
	bShowMouseCursor   = true;
	DefaultMouseCursor = EMouseCursor::Default;

	// 边缘滚动默认设置
	EdgeScrollThickness = 50.0f;
	EdgeScrollSpeed     = 200.0f;
	CameraMoveSpeed     = 1000.0f;

	MoveInputY = 0.0f;
	MoveInputX = 0.0f;

	SelectionBox.IsActive = false;
}

USpringArmComponent* AEndCraftPlayerController::GetSpringArmComponent() const
{
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn)
	{
		AEndCraftCameraPawn* CameraPawn = Cast<AEndCraftCameraPawn>(ControlledPawn);
		if (CameraPawn)
		{
			return CameraPawn->GetSpringArmComponent();
		}
	}
	return nullptr;
}

void AEndCraftPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 获取 Enhanced Input 子系统并添加映射
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		GetLocalPlayer()))
	{
		if (DefaultMappingContext)
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AEndCraftPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 每帧处理相机移动（包括 WASD 和边缘滚动）
	MoveCamera();
}

void AEndCraftPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// 转换为 Enhanced Input Component
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// 绑定 WASD 移动 - 使用 Started 处理按下，Completed 处理释放
		if (IA_MoveUp)
		{
			EnhancedInputComponent->BindAction(IA_MoveUp, ETriggerEvent::Started, this,
			                                   &AEndCraftPlayerController::OnMoveUpStarted);
			EnhancedInputComponent->BindAction(IA_MoveUp, ETriggerEvent::Completed, this,
			                                   &AEndCraftPlayerController::OnMoveUpCompleted);
		}
		if (IA_MoveDown)
		{
			EnhancedInputComponent->BindAction(IA_MoveDown, ETriggerEvent::Started, this,
			                                   &AEndCraftPlayerController::OnMoveDownStarted);
			EnhancedInputComponent->BindAction(IA_MoveDown, ETriggerEvent::Completed, this,
			                                   &AEndCraftPlayerController::OnMoveDownCompleted);
		}
		if (IA_MoveLeft)
		{
			EnhancedInputComponent->BindAction(IA_MoveLeft, ETriggerEvent::Started, this,
			                                   &AEndCraftPlayerController::OnMoveLeftStarted);
			EnhancedInputComponent->BindAction(IA_MoveLeft, ETriggerEvent::Completed, this,
			                                   &AEndCraftPlayerController::OnMoveLeftCompleted);
		}
		if (IA_MoveRight)
		{
			EnhancedInputComponent->BindAction(IA_MoveRight, ETriggerEvent::Started, this,
			                                   &AEndCraftPlayerController::OnMoveRightStarted);
			EnhancedInputComponent->BindAction(IA_MoveRight, ETriggerEvent::Completed, this,
			                                   &AEndCraftPlayerController::OnMoveRightCompleted);
		}

		// 绑定相机缩放
		if (IA_ZoomCamera)
		{
			EnhancedInputComponent->BindAction(IA_ZoomCamera, ETriggerEvent::Triggered, this,
			                                   &AEndCraftPlayerController::ZoomCamera);
		}

		// 绑定单位选择 - 使用 Started 和 Ongoing 实现框选
		if (IA_SelectUnits)
		{
			EnhancedInputComponent->BindAction(IA_SelectUnits, ETriggerEvent::Started, this,
			                                   &AEndCraftPlayerController::OnSelectStart);
			EnhancedInputComponent->BindAction(IA_SelectUnits, ETriggerEvent::Ongoing, this,
			                                   &AEndCraftPlayerController::OnSelectUpdate);
			EnhancedInputComponent->BindAction(IA_SelectUnits, ETriggerEvent::Completed, this,
			                                   &AEndCraftPlayerController::OnSelectEnd);
		}

		// 绑定时停
		if (IA_TimeStop)
		{
			EnhancedInputComponent->BindAction(IA_TimeStop, ETriggerEvent::Triggered, this,
			                                   &AEndCraftPlayerController::ToggleTimeStop);
		}

		// 绑定右键移动命令
		if (IA_MoveCommand)
		{
			EnhancedInputComponent->BindAction(IA_MoveCommand, ETriggerEvent::Completed, this,
			                                   &AEndCraftPlayerController::IssueMoveCommand);
		}
	}
}

// W 键按下 - 向上移动（Y 轴正向）
void AEndCraftPlayerController::OnMoveUpStarted(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT(__FUNCTION__));
	MoveInputY = 1.0f;
}

void AEndCraftPlayerController::OnMoveUpCompleted(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT(__FUNCTION__));
	if (MoveInputY > 0) MoveInputY = 0.0f;
}

// S 键按下 - 向下移动（Y 轴负向）
void AEndCraftPlayerController::OnMoveDownStarted(const FInputActionValue& Value)
{
	MoveInputY = -1.0f;
}

void AEndCraftPlayerController::OnMoveDownCompleted(const FInputActionValue& Value)
{
	if (MoveInputY < 0) MoveInputY = 0.0f;
}

// A 键按下 - 向左移动（X 轴负向）
void AEndCraftPlayerController::OnMoveLeftStarted(const FInputActionValue& Value)
{
	MoveInputX = -1.0f;
}

void AEndCraftPlayerController::OnMoveLeftCompleted(const FInputActionValue& Value)
{
	if (MoveInputX < 0) MoveInputX = 0.0f;
}

// D 键按下 - 向右移动（X 轴正向）
void AEndCraftPlayerController::OnMoveRightStarted(const FInputActionValue& Value)
{
	MoveInputX = 1.0f;
}

void AEndCraftPlayerController::OnMoveRightCompleted(const FInputActionValue& Value)
{
	if (MoveInputX > 0) MoveInputX = 0.0f;
}

void AEndCraftPlayerController::MoveCamera()
{
	float TotalMoveY = MoveInputY;
	float TotalMoveX = MoveInputX;

	// 添加边缘滚动输入
	FVector2D EdgeScrollInput = CalculateEdgeScrollInput();
	TotalMoveY += EdgeScrollInput.Y;
	TotalMoveX += EdgeScrollInput.X;

	// 如果没有输入，不移动
	if (FMath::IsNearlyZero(TotalMoveY) && FMath::IsNearlyZero(TotalMoveX))
	{
		return;
	}

	// 获取控制的 Pawn（相机 Pawn）
	AEndCraftCameraPawn* CameraPawn = Cast<AEndCraftCameraPawn>(GetPawn());
	if (CameraPawn)
	{
		// 使用相机 Pawn 的移动函数
		CameraPawn->AddCameraMovement(FVector(TotalMoveX, TotalMoveY, 0.0f), CameraMoveSpeed);
	}
}

FVector2D AEndCraftPlayerController::CalculateEdgeScrollInput() const
{
	FVector2D EdgeScrollInput = FVector2D::ZeroVector;

	// 获取鼠标位置
	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);

	FVector2D MousePosition;
	GetMousePosition(MousePosition.X, MousePosition.Y);

	float Width  = static_cast<float>(ViewportSize.X);
	float Height = static_cast<float>(ViewportSize.Y);

	// 计算边缘滚动强度
	float ScrollX = 0.0f;
	float ScrollY = 0.0f;

	// 左边缘（A 方向）
	if (MousePosition.X < EdgeScrollThickness)
	{
		ScrollX = -abs(GetEdgeScrollIntensity(MousePosition.X, EdgeScrollThickness));
	}
	// 右边缘（D 方向）
	else if (MousePosition.X > Width - EdgeScrollThickness)
	{
		ScrollX = abs(GetEdgeScrollIntensity(Width - MousePosition.X, EdgeScrollThickness));
	}

	// 上边缘（W 方向）
	if (MousePosition.Y < EdgeScrollThickness)
	{
		ScrollY = abs(-GetEdgeScrollIntensity(MousePosition.Y, EdgeScrollThickness));
	}
	// 下边缘（S 方向）
	else if (MousePosition.Y > Height - EdgeScrollThickness)
	{
		ScrollY = -abs(GetEdgeScrollIntensity(Height - MousePosition.Y, EdgeScrollThickness));
	}

	EdgeScrollInput.X = ScrollX * EdgeScrollSpeed / 100.0f;
	EdgeScrollInput.Y = ScrollY * EdgeScrollSpeed / 100.0f;

	return EdgeScrollInput;
}

float AEndCraftPlayerController::GetEdgeScrollIntensity(float Position, float EdgeThickness) const
{
	// 越靠近边缘，滚动速度越快（线性插值）
	// Position=0 时返回 1.0，Position=EdgeThickness 时返回 0.0
	return 1.0f - (Position / EdgeThickness);
}

void AEndCraftPlayerController::ZoomCamera(const FInputActionValue& Value)
{
	const float ZoomDelta = Value.Get<float>();

	USpringArmComponent* SpringArm = GetSpringArmComponent();
	if (SpringArm)
	{
		float CurrentLength        = SpringArm->TargetArmLength;
		float NewLength            = FMath::Clamp(CurrentLength - ZoomDelta * 100.0f, 500.0f, 3000.0f);
		SpringArm->TargetArmLength = NewLength;
	}
}

// 框选开始
void AEndCraftPlayerController::OnSelectStart(const FInputActionValue& Value)
{
	FVector2D MousePos;
	GetMousePosition(MousePos.X, MousePos.Y);

	SelectionBox.StartPos = MousePos;
	SelectionBox.EndPos   = MousePos;
	SelectionBox.IsActive = true;

	UE_LOG(LogTemp, Log, TEXT("Selection Start: %s"), *MousePos.ToString());
}

// 框选更新
void AEndCraftPlayerController::OnSelectUpdate(const FInputActionValue& Value)
{
	if (!SelectionBox.IsActive) return;

	FVector2D MousePos;
	GetMousePosition(MousePos.X, MousePos.Y);

	SelectionBox.EndPos = MousePos;

	// 后续可以在这里更新框选 UI 显示
}

// 框选结束
void AEndCraftPlayerController::OnSelectEnd(const FInputActionValue& Value)
{
	if (!SelectionBox.IsActive) return;

	SelectionBox.IsActive = false;

	FVector2D MousePos;
	GetMousePosition(MousePos.X, MousePos.Y);
	SelectionBox.EndPos = MousePos;

	// 判断是框选还是单选
	float DragDistance = FVector2D::Distance(SelectionBox.StartPos, SelectionBox.EndPos);

	if (DragDistance < 5.0f)
	{
		// 点击距离很小，视为单选
		PerformSingleSelection();
	}
	else
	{
		// 框选
		PerformBoxSelection();
	}

	UE_LOG(LogTemp, Log, TEXT("Selection End: %s"), *MousePos.ToString());
}

void AEndCraftPlayerController::PerformSingleSelection()
{
	// 清除之前的选择
	ClearAllSelections();

	// 射线检测获取单位
	AEndCraftHeroCharacter* Hero = GetHeroUnderCursor();
	if (Hero)
	{
		Hero->SetSelected(true);
		UE_LOG(LogTemp, Log, TEXT("Selected hero: %s"), *Hero->GetName());
	}
}

void AEndCraftPlayerController::PerformBoxSelection()
{
	// 清除之前的选择
	ClearAllSelections();

	// 获取所有英雄单位
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(this, AEndCraftHeroCharacter::StaticClass(), FoundActors);

	int SelectedCount = 0;
	for (AActor* Actor : FoundActors)
	{
		AEndCraftHeroCharacter* Hero = Cast<AEndCraftHeroCharacter>(Actor);
		if (!Hero) continue;

		// 将英雄位置投影到屏幕空间
		FVector   HeroLocation = Hero->GetActorLocation();
		FVector2D ScreenPosition;
		if (ProjectWorldLocationToScreen(HeroLocation, ScreenPosition))
		{
			// 检查是否在框选范围内
			if (SelectionBox.Contains(ScreenPosition))
			{
				Hero->SetSelected(true);
				SelectedCount++;
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Box selection: %d heroes selected"), SelectedCount);
}

void AEndCraftPlayerController::ClearAllSelections()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(this, AEndCraftHeroCharacter::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		AEndCraftHeroCharacter* Hero = Cast<AEndCraftHeroCharacter>(Actor);
		if (Hero && Hero->IsSelected())
		{
			Hero->SetSelected(false);
		}
	}
}

AEndCraftHeroCharacter* AEndCraftPlayerController::GetHeroUnderCursor() const
{
	FVector WorldLocation;
	if (!GetMouseWorldLocation(WorldLocation))
	{
		return nullptr;
	}

	// 射线检测
	FHitResult            HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	UWorld* World = GetWorld();
	if (World->LineTraceSingleByChannel(HitResult, WorldLocation,
	                                    WorldLocation - FVector(0, 0, 1000.0f), ECC_Visibility, QueryParams))
	{
		return Cast<AEndCraftHeroCharacter>(HitResult.GetActor());
	}

	return nullptr;
}

bool AEndCraftPlayerController::GetMouseWorldLocation(FVector& OutLocation) const
{
	FVector2D MousePosition;
	if (!GetMousePosition(MousePosition.X, MousePosition.Y))
	{
		return false;
	}

	FVector WorldOrigin, WorldDirection;
	if (DeprojectScreenPositionToWorld(MousePosition.X, MousePosition.Y,
	                                   WorldOrigin, WorldDirection))
	{
		// 与 XY 平面（Z=0）相交
		FPlane  Plane(FVector::ZeroVector, FVector::UpVector);
		FVector IntersectionPoint;
		if (FMath::SegmentPlaneIntersection(WorldOrigin,
		                                    WorldOrigin + WorldDirection * 10000.0f, Plane, IntersectionPoint))
		{
			OutLocation = IntersectionPoint;
			return true;
		}
	}

	return false;
}

void AEndCraftPlayerController::IssueMoveCommand()
{
	// 获取鼠标点击的世界位置
	FVector MouseWorldLocation;
	if (!GetMouseWorldLocation(MouseWorldLocation))
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Move command issued to location: %s"), *MouseWorldLocation.ToString());

	// 检查是否按住 Shift 键
	bool bShiftDown = IsShiftKeyDown();

	// 获取所有选中的单位
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(this, AEndCraftHeroCharacter::StaticClass(), FoundActors);

	int CommandCount = 0;
	for (AActor* Actor : FoundActors)
	{
		AEndCraftHeroCharacter* Hero = Cast<AEndCraftHeroCharacter>(Actor);
		if (Hero && Hero->IsSelected())
		{
			// 如果没按 Shift，清空当前队列
			if (!bShiftDown)
			{
				Hero->ClearCommandQueue(false);
			}
			// 创建移动指令并添加到队列
			FCommandQueueItem MoveCommand = FCommandQueueItem::CreateMoveCommand(MouseWorldLocation);
			Hero->AddCommand(MoveCommand);
			CommandCount++;
		}
	}

	if (bShiftDown)
	{
		UE_LOG(LogTemp, Log, TEXT("Move command queued to %d heroes"), CommandCount);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Move command sent to %d heroes (queue cleared)"), CommandCount);
	}
}

bool AEndCraftPlayerController::IsShiftKeyDown() const
{
    return IsInputKeyDown(EKeys::LeftShift) || IsInputKeyDown(EKeys::RightShift);
}

void AEndCraftPlayerController::SelectUnits(const FInputActionValue& Value)
{
	// 此函数现在由 OnSelectStart/Update/End 处理
	UE_LOG(LogTemp, Log, TEXT("Select Units Input Triggered"));
}

void AEndCraftPlayerController::ToggleTimeStop(const FInputActionValue& Value)
{
	// 获取 GameState
	AEndCraftGameState* GS = Cast<AEndCraftGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		if (GS->IsTimeStopActive())
		{
			GS->ExitTimeStop();
		}
		else
		{
			GS->EnterTimeStop();
		}
	}
}
