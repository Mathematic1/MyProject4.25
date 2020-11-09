// Fill out your copyright notice in the Description page of Project Settings.


#include "PunchEndNotify.h"
#include "BatteryMan.h"

void UPunchEndNotify::Notify(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	if (MeshComp != NULL && MeshComp->GetOwner() != NULL) {
		ABatteryMan* Player = Cast<ABatteryMan>(MeshComp->GetOwner());
		if (Player != NULL) {
			Player->FinishAttack();
		}
	}
}
