// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RotationActor.generated.h"

class UBoxComponent;

UCLASS()
class MYPROJECT2_API ARotationActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARotationActor();

	UPROPERTY(EditAnywhere)
		FRotator RotationInTick;

	UFUNCTION()
		FRotator GetRotation();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* MeshCollision;

	UFUNCTION()
		void OnBeginOverlap(class UPrimitiveComponent* HitComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComponent,
			int32 OtherBodyIndex, bool bFroomSweep, const FHitResult &SweepResult);

	UFUNCTION()
		void OnEndOverlap(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TArray<UMaterialInterface*> matInterface;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UBoxComponent* boxCollision;

	UFUNCTION()
		void OnPlace(AActor* OverlappedActor, AActor* OtherActor);

	FTimerHandle timer;
	FVector directionOfPlatform;

	void SetActorRotate(FRotator ActorRotation);
	void upPlatform();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
