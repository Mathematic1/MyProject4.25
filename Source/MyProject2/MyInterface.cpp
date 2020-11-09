// Fill out your copyright notice in the Description page of Project Settings.


#include "MyInterface.h"
#include "ProjectileActor.h"

// Add default functionality here for any IMyInterface functions that are not pure virtual.

void IMyInterface::timeForDestroy(AActor* actor)
{
	AProjectileActor* projectileActor = Cast<AProjectileActor>(actor);
	//projectileActor->projectileMovement->bShouldBounce = false;
	//projectileActor->projectileMovement->bSimulationEnabled = false;
	UE_LOG(LogTemp, Warning, TEXT("Projectile start"));
}
