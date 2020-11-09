// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT2_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimInstance")
		bool IsInAir;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimInstance")
		bool IsAnimationBlended;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimInstance")
		float Speed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimInstance")
		float MoveDirection;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimInstance")
		bool bIsArmed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimInstance")
		bool bIsCrouching;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimInstance")
		bool bIsMoving;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimInstance")
		bool bIsRunning;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimInstance")
		float MoveRight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimInstance")
		float MoveForward;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimInstance")
		bool IsRoll;

	UFUNCTION(BlueprintImplementableEvent)
	void SetRootMode();
	void SetRootMode_Implementation();

public:

	UPlayerAnimInstance();

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	APawn* Own;
};
