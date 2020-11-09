// Fill out your copyright notice in the Description page of Project Settings.


#include "ToggleProp.h"
#include "Engine.h"
#include "Engine/Public/EngineUtils.h"
#include "Components/StaticMeshComponent.h"

AToggleProp::AToggleProp() {
	bCanScale = false;
}

void AToggleProp::Interact_Implementation()
{
	for (TActorIterator<AActor> ActorIterator(GetWorld()); ActorIterator; ++ActorIterator) {
		AActor* Actor = *ActorIterator;
		if (Actor&&Actor!=this&&Actor->GetClass()->ImplementsInterface(UInterractiveActor::StaticClass())) {
			/*for (FName Tag : Actor->Tags) {
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, Tag.ToString());
			}
			if (Actor->ActorHasTag(ToggleTag)) {
				IInterractiveActor::Execute_Interact(Actor);
			}*/
			for (FName TagToCheck : ToggleTags) {
				if (Actor->ActorHasTag(TagToCheck)) {
					IInterractiveActor::Execute_Interact(Actor);
				}
			}
		}
	}
}
