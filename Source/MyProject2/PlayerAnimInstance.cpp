// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"
#include "BatteryMan.h"

void UPlayerAnimInstance::SetRootMode_Implementation()
{
}

UPlayerAnimInstance::UPlayerAnimInstance()
{
	IsInAir = false;
	IsAnimationBlended = true;
	Speed = 0.f;
	IsRoll = false;
}

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Own = TryGetPawnOwner();
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if(!Own) {
		return;
	}
	if (Own->IsA(ABatteryMan::StaticClass())) {
		ABatteryMan* OwnerPlayer = Cast<ABatteryMan>(Own);
		if (OwnerPlayer) {
			IsInAir = OwnerPlayer->GetMovementComponent()->IsFalling();
			IsAnimationBlended = OwnerPlayer->GetIsAnimationBlended();
			Speed = OwnerPlayer->MovingSpeed;
			MoveDirection = OwnerPlayer->InputMoveDirection;
			bIsCrouching = OwnerPlayer->GetMovementComponent()->IsCrouching();
			bIsArmed = OwnerPlayer->IsArmed();
			bIsMoving = OwnerPlayer->MovingSpeed > 0 ? true : false;

			bIsRunning = OwnerPlayer->IsRunning();
			IsRoll = OwnerPlayer->GetAnimRoll();
		}
	}
}
