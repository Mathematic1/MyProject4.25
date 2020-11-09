// Fill out your copyright notice in the Description page of Project Settings.


#include "EndRollNotify.h"
#include "BatteryMan.h"

void UEndRollNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) {
	if (MeshComp != NULL && MeshComp->GetOwner() != NULL) {
		ABatteryMan* Player = Cast<ABatteryMan>(MeshComp->GetOwner());
		if (Player != NULL) {
			Player->SetAnimRoll(false);
			Player->SetIsKeyboardEnabled(true);
			Player->bRollFalling = false;
			Player->StopRollMontage();
		}
	}
}

