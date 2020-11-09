// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Blueprint/UserWidget.h"
#include "ProjectPlayerState.generated.h"

/**
 * 
 */

/*USTRUCT(BlueprintType)
struct FMessageInfoCurrent {
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Name = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Message = "";

	FMessageInfoCurrent() {
		Name = "";
		Message = "";
	}
};*/

UCLASS()
class MYPROJECT2_API AProjectPlayerState : public APlayerState
{
	GENERATED_BODY()

		AProjectPlayerState();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ComboCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PlayerName;
};
