// Fill out your copyright notice in the Description page of Project Settings.


#include "PunchThrowAnimNotify.h"
#include "BatteryMan.h"
#include "Engine.h"

void UPunchThrowAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) {
	GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Orange, __FUNCTION__);
	/*if (MeshComp != NULL && MeshComp->GetOwner() != NULL) {
		ABatteryMan* Player = Cast<ABatteryMan>(MeshComp->GetOwner());
		if (Player != NULL&&!Player->punchAudioThrowComponent->IsPlaying()) {
			Player->punchAudioThrowComponent->Play(0.f);
		}
	}*/
}
