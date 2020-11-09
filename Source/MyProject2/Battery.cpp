// Fill out your copyright notice in the Description page of Project Settings.


#include "Battery.h"
#include "Components/BoxComponent.h"

// Sets default values
ABattery::ABattery()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetSimulatePhysics(true);
	RootComponent = Mesh;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetupAttachment(RootComponent);
	BoxCollision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	FCollisionResponseContainer collisionContainer;
	collisionContainer.SetAllChannels(ECollisionResponse::ECR_Block);
	collisionContainer.SetResponse(ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	collisionContainer.SetResponse(ECC_PhysicsBody, ECollisionResponse::ECR_Overlap);
	BoxCollision->SetCollisionResponseToChannels(collisionContainer);
}

// Called when the game starts or when spawned
void ABattery::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABattery::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

