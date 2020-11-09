// Fill out your copyright notice in the Description page of Project Settings.


#include "BatteryMan_GameMode.h"
#include "ProjectPlayerController.h"
#include "ProjectGameState.h"
#include "GameFramework/Actor.h"


ABatteryMan_GameMode::ABatteryMan_GameMode() {

	PrimaryActorTick.bCanEverTick = true;

}

void ABatteryMan_GameMode::BeginPlay() {

	Super::BeginPlay();

	/*FTimerHandle UnusedHandle;
	GetWorldTimerManager().SetTimer(UnusedHandle, this, &ABatteryMan_GameMode::SpawnPlayerRecharge, FMath::RandRange(2, 5), true);

	FTimerHandle UnusedJumpHandle;
	GetWorldTimerManager().SetTimer(UnusedJumpHandle, this, &ABatteryMan_GameMode::SpawnPlayerJumper, FMath::RandRange(10, 15), true);*/
}

void ABatteryMan_GameMode::Tick(float DeltaTime) {

	Super::Tick(DeltaTime);
	GetProjectGameState();
}

AProjectGameState * ABatteryMan_GameMode::GetProjectGameState()
{
	AProjectGameState* GS = GetGameState<AProjectGameState>();
	return GS;
}

/*void ABatteryMan_GameMode::SpawnPlayerRecharge()
{

	float RandX = FMath::RandRange(Spawn_X_Min, Spawn_X_Max);
	float RandY = FMath::RandRange(Spawn_Y_Min, Spawn_Y_Max);

	FVector SpawnPosition = FVector(RandX, RandY, Spawn_Z);
	FRotator SpawnRotation = FRotator(0.0f, 0.0f, 0.0f);

	GetWorld()->SpawnActor(PlayerRecharge, &SpawnPosition, &SpawnRotation);

}

void ABatteryMan_GameMode::SpawnPlayerJumper()
{

	float RandX = FMath::RandRange(Spawn_X_Min, Spawn_X_Max);
	float RandY = FMath::RandRange(Spawn_Y_Min, Spawn_Y_Max);

	FVector SpawnPosition = FVector(RandX, RandY, Spawn_Z);

	float RotX = FMath::RandRange(0.0f, 360.0f);
	float RotY = FMath::RandRange(0.0f, 360.0f);
	float RotZ = FMath::RandRange(0.0f, 360.0f);

	FRotator SpawnRotation = FRotator(RotX, RotY, RotZ);

	GetWorld()->SpawnActor(PlayerJumper, &SpawnPosition, &SpawnRotation);

}*/

