// Copyright Epic Games, Inc. All Rights Reserved.

#include "EndCraftHeroCharacter.h"

#include "AIController.h"
#include "AITypes.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"


AEndCraftHeroCharacter::AEndCraftHeroCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// ensure this unit has a valid AI controller to handle move requests
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// 胶囊体
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	if (CapsuleComp)
	{
		CapsuleComp->InitCapsuleSize(42.0f, 96.0f);
		CapsuleComp->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	}

	// 创建选中指示器（光圈）
	SelectionCircleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SelectionCircleComponent"));
	SelectionCircleComponent->SetupAttachment(RootComponent);
	SelectionCircleComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	SelectionCircleComponent->SetRelativeScale3D(FVector(1.5f, 1.5f, 1.0f));
	SelectionCircleComponent->SetVisibility(false);

	// 移动组件
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (MoveComp)
	{
		MoveComp->GravityScale                 = 1.5f;
		MoveComp->BrakingFrictionFactor        = 1.0f;
		MoveComp->bConstrainToPlane            = true;
		MoveComp->bOrientRotationToMovement    = true;
		MoveComp->AvoidanceConsiderationRadius = 150.0f;
		MoveComp->AvoidanceWeight              = 1.0f;
		MoveComp->RotationRate                 = FRotator(0.0f, 500.0f, 0.0f);
		MoveComp->MaxWalkSpeed                 = 350.0f;
		MoveComp->MaxAcceleration              = 1000.0f;
		MoveComp->BrakingDecelerationWalking   = 1000.0f;
	}

	// 默认属性
	bIsSelected         = false;
	bIsExecutingCommand = false;
	CommandTimer        = 0.0f;
}

void AEndCraftHeroCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AEndCraftHeroCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 处理指令队列
	ProcessCommandQueue(DeltaTime);
}

void AEndCraftHeroCharacter::SetSelected(bool bInSelected)
{
	bIsSelected = bInSelected;

	if (SelectionCircleComponent)
	{
		SelectionCircleComponent->SetVisibility(bIsSelected);
	}
}

void AEndCraftHeroCharacter::AddCommand(const FCommandQueueItem& Command)
{
	if (Command.IsValid())
	{
		CommandQueue.Add(Command);
		UE_LOG(LogTemp, Log, TEXT("Hero %s added command: %d, Queue size: %d"),
		       *GetName(), (int32)Command.CommandType, CommandQueue.Num());
	}
}

void AEndCraftHeroCharacter::ClearCommandQueue(bool bStopPhysicalMovement)
{
	CommandQueue.Empty();
	CurrentCommand      = FCommandQueueItem();
	bIsExecutingCommand = false;
	CommandTimer        = 0.0f;

	// 【关键修改】按需刹车
	if (bStopPhysicalMovement)
	{
		GetCharacterMovement()->StopMovementImmediately();
		if (AIController) AIController->StopMovement();
	}
	UE_LOG(LogTemp, Log, TEXT("Hero %s command queue cleared"), *GetName());
}

void AEndCraftHeroCharacter::ProcessCommandQueue(float DeltaTime)
{
	// 如果正在执行指令，更新计时器
	if (bIsExecutingCommand)
	{
		CommandTimer += DeltaTime;

		// 根据指令类型检查是否完成
		switch (CurrentCommand.CommandType)
		{
		case ECommandType::Move:
			{
				// 移动指令由 OnMoveCompleted 事件处理，不在这里检查
				break;
			}

		case ECommandType::Wait:
			{
				// 等待指定时间
				if (CommandTimer >= CurrentCommand.WaitTime)
				{
					CompleteCurrentCommand();
				}
				break;
			}

		case ECommandType::Attack:
			{
				// 攻击指令（后续实现）
				// 暂时简单处理：到达目标后完成
				if (CurrentCommand.TargetActor.IsValid())
				{
					float DistanceToTarget = FVector::Dist2D(GetActorLocation(),
					                                         CurrentCommand.TargetActor->GetActorLocation());
					if (DistanceToTarget < 100.0f) // 攻击范围
					{
						CompleteCurrentCommand();
					}
				}
				else
				{
					// 目标已失效，跳过
					CompleteCurrentCommand();
				}
				break;
			}

		default:
			CompleteCurrentCommand();
			break;
		}

		return;
	}

	// 没有正在执行的指令，检查队列是否有新指令
	if (CommandQueue.Num() > 0)
	{
		// 取出队首指令
		CurrentCommand = CommandQueue[0];
		CommandQueue.RemoveAt(0);

		bIsExecutingCommand = true;
		CommandTimer        = 0.0f;

		// 执行指令
		ExecuteCommand(CurrentCommand);

		UE_LOG(LogTemp, Log, TEXT("Hero %s executing command: %d"),
		       *GetName(), (int32)CurrentCommand.CommandType);
	}
}

#pragma optimize("", off)
void AEndCraftHeroCharacter::ExecuteCommand(const FCommandQueueItem& Command)
{
	AIController = Cast<AAIController>(Controller);
	if (!AIController)
	{
		return;
	}
	switch (Command.CommandType)
	{
	case ECommandType::Move:
		{
			// 绑定移动完成事件
			AIController->ReceiveMoveCompleted.AddDynamic(this, &AEndCraftHeroCharacter::OnMoveCompleted);

			// 移动到目标位置
			FAIMoveRequest MoveReq;
			MoveReq.SetGoalLocation(Command.TargetLocation);
			MoveReq.SetAcceptanceRadius(150.f);
			MoveReq.SetAllowPartialPath(true);
			MoveReq.SetUsePathfinding(true);
			MoveReq.SetProjectGoalLocation(true);
			MoveReq.SetRequireNavigableEndLocation(true);
			MoveReq.SetNavigationFilter(AIController->GetDefaultNavigationFilterClass());
			MoveReq.SetCanStrafe(false);

			FNavPathSharedPtr                 FollowedPath;
			const FPathFollowingRequestResult ResultData = AIController->MoveTo(MoveReq, &FollowedPath);

			break;
		}

	case ECommandType::Attack:
		{
			// 移动到攻击目标附近
			if (Command.TargetActor.IsValid())
			{
				FVector Direction = (Command.TargetActor->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
				AddMovementInput(Direction, 1.0f);
			}
			break;
		}

	case ECommandType::Wait:
		{
			// 等待，不移动
			GetCharacterMovement()->StopMovementImmediately();
			break;
		}

	default:
		break;
	}
}

void AEndCraftHeroCharacter::CompleteCurrentCommand()
{
	UE_LOG(LogTemp, Log, TEXT("Hero %s completed command"), *GetName());

	// 重置状态（注意顺序）
	bIsExecutingCommand = false;
	CommandTimer        = 0.0f;
	CurrentCommand      = FCommandQueueItem();
}
#pragma optimize("", on)

void AEndCraftHeroCharacter::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (AIController)
	{
		AIController->ReceiveMoveCompleted.RemoveDynamic(this, &AEndCraftHeroCharacter::OnMoveCompleted);
	}

	// 检查结果是否成功
	if (Result == EPathFollowingResult::Success)
	{
		// 移动成功，完成当前指令
		CompleteCurrentCommand();
	}
	else
	{
		// 移动失败，记录日志并完成指令（避免卡住）
		UE_LOG(LogTemp, Warning, TEXT("Hero %s move failed: %s"), *GetName(), *UEnum::GetValueAsString(Result));
		CompleteCurrentCommand();
	}
}
