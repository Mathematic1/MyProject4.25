// Fill out your copyright notice in the Description page of Project Settings.


#include "BatteryPicable.h"

// Sets default values
ABatteryPicable::ABatteryPicable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABatteryPicable::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABatteryPicable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

