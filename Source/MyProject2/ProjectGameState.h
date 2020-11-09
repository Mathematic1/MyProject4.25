// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "ProjectGameState.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FMessageInfo {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Name = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Message = "";

	FMessageInfo() {
		Name = "";
		Message = "";
	}
};


UCLASS()
class MYPROJECT2_API AProjectGameState : public AGameState
{
	GENERATED_BODY()

		AProjectGameState();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AllComboCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_UpdateMessages)
	TArray<FMessageInfo> Messages;

	UFUNCTION()
	void OnRep_UpdateMessages();

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateMessagesOnClient(FMessageInfo Message);
	void UpdateMessagesOnClient_Implementation(FMessageInfo Message);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
