// Fill out your copyright notice in the Description page of Project Settings.


#include "DestructibleProp.h"
#include "Components/BoxComponent.h"

// Sets default values
ADestructibleProp::ADestructibleProp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Component"));

	DestructibleComponent = CreateDefaultSubobject<UDestructibleComponent>(TEXT("Destructible Component"));
	DestructibleComponent->SetupAttachment(RootComponent);

	TriggerComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerComponent"));
	TriggerComponent->SetupAttachment(RootComponent);

	IsDestroyed = false;
	IsTriggerEnabled = false;
	MaxHealth = 10.f;
	DefaultDamage = 1.f;
	DefaultImpulse = 1.f;
	TriggerCountdown = 10;
}

void ADestructibleProp::Damage(UPrimitiveComponent * HitComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{
	if (!IsDestroyed&&OtherComp->ComponentHasTag("Weapon")) {
		GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Orange, __FUNCTION__);
		CurrentHealth = CurrentHealth - 1.f;
		if (CurrentHealth <= 0.f) {
			Destroy(DefaultDamage, Hit.Location, NormalImpulse, DefaultImpulse);
		}
	}
}

void ADestructibleProp::Trigger(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (!IsDestroyed&&IsTriggerEnabled&&OtherActor->ActorHasTag("Player")) {
		if (GetWorld()->GetTimerManager().IsTimerActive(TriggerDestroyTimerHandle)) {
			GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green, "Pausing Timer");
			GetWorld()->GetTimerManager().ClearTimer(TriggerDestroyTimerHandle);
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green, "Starting Timer");
			GetWorld()->GetTimerManager().SetTimer(TriggerDestroyTimerHandle, this, &ADestructibleProp::TriggerCountdownDestroy, 1.f, true);
			TriggerCountdown = FMath::RandRange(1, 10);
		}
	}
}

void ADestructibleProp::Destroy(float Damage, FVector HitLocation, FVector ImpulseDir, float Impulse)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green, __FUNCTION__);

	if (!IsDestroyed) {
		IsDestroyed = true;

		DestructibleComponent->ApplyDamage(Damage, HitLocation, ImpulseDir, Impulse);
	}
}

void ADestructibleProp::TriggerDestroy()
{
	if (!IsDestroyed&&IsTriggerEnabled) {

		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Orange, "About explode");
		Destroy(DefaultDamage, DestructibleComponent->GetComponentLocation(), DestructibleComponent->GetUpVector(), DefaultImpulse);
	}
}

void ADestructibleProp::TriggerCountdownDestroy()
{
	if (--TriggerCountdown <= 0) {
		int32 RandomDelay = FMath::RandRange(1, 4);
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Orange, "Trigger explosion after: " + FString::FromInt(RandomDelay) + "seconds");
		GetWorld()->GetTimerManager().SetTimer(TriggerDestroyTimerHandle, this, &ADestructibleProp::TriggerDestroy, RandomDelay, false);
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Orange, "Counting down: " + FString::FromInt(TriggerCountdown));
	}
}

void ADestructibleProp::Interact_Implementation()
{
	TriggerDestroy();
}

// Called when the game starts or when spawned
void ADestructibleProp::BeginPlay()
{
	Super::BeginPlay();
	
	DestructibleComponent->OnComponentHit.AddDynamic(this, &ADestructibleProp::Damage);

	TriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &ADestructibleProp::Trigger);
	
	CurrentHealth = MaxHealth;
}

// Called every frame
void ADestructibleProp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

