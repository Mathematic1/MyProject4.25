// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "ProjectGameState.h"
#include "BatteryMan_GameMode.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT2_API ABatteryMan_GameMode : public AGameMode
{
	GENERATED_BODY()

		ABatteryMan_GameMode();

 	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

public:
	AProjectGameState* GetProjectGameState();

	/*UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> PlayerRecharge;

	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> PlayerJumper;

	float Spawn_Z = 500.0f;

	UPROPERTY(EditAnywhere)
		float Spawn_X_Min;

	UPROPERTY(EditAnywhere)
		float Spawn_X_Max;

	UPROPERTY(EditAnywhere)
		float Spawn_Y_Min;

	UPROPERTY(EditAnywhere)
		float Spawn_Y_Max;

	void SpawnPlayerRecharge();

	void SpawnPlayerJumper();*/

};
