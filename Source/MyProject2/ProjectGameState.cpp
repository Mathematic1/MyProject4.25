// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectGameState.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"

AProjectGameState::AProjectGameState() {

}

void AProjectGameState::OnRep_UpdateMessages()
{
	UpdateMessagesOnClient(Messages[Messages.Num()-1]);
}

void AProjectGameState::UpdateMessagesOnClient_Implementation(FMessageInfo Message)
{
}

void AProjectGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AProjectGameState, Messages);
}


