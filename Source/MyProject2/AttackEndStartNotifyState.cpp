// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackEndStartNotifyState.h"
#include "BatteryMan.h"
#include "Engine.h"

void UAttackEndStartNotifyState::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) {
	GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Red, __FUNCTION__);
	if (MeshComp != NULL && MeshComp->GetOwner() != NULL) {
		ABatteryMan* Player = Cast<ABatteryMan>(MeshComp->GetOwner());
		if (Player!=NULL) {
			Player->AttackStart();
		}
	}
}
void UAttackEndStartNotifyState::NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime)
{
	if (MeshComp != NULL && MeshComp->GetOwner() != NULL) {
		ABatteryMan* Player = Cast<ABatteryMan>(MeshComp->GetOwner());
		if (Player != NULL) {
			if (Player->GetCurrentAttack() == EAttackType::MELEE_KICK) {
				//Player->SetIsKeyboardEnabled(false);
			}
		}
	}
}
void UAttackEndStartNotifyState::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) {
	GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Red, __FUNCTION__);
	if (MeshComp != NULL && MeshComp->GetOwner() != NULL) {
		ABatteryMan* Player = Cast<ABatteryMan>(MeshComp->GetOwner());
		if (Player != NULL) {
			Player->SetIsKeyboardEnabled(true);
			Player->AttackEnd();
		}
	}
}