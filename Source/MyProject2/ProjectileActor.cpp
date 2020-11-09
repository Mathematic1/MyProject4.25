// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileActor.h"

// Sets default values
AProjectileActor::AProjectileActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	projectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	projectileMovement->bInitialVelocityInLocalSpace = true;
	projectileMovement->Velocity = FVector(1.f, 1.f, 0.f);
	projectileMovement->ProjectileGravityScale = 0.f;
	projectileMovement->InitialSpeed = 600.f;
	projectileMovement->MaxSpeed = 1200.f;
	projectileMovement->bShouldBounce = 1;

	time = 1;
}

// Called when the game starts or when spawned
void AProjectileActor::BeginPlay()
{
	Super::BeginPlay();
	
	timeForDestroy(this);
}

// Called every frame
void AProjectileActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

