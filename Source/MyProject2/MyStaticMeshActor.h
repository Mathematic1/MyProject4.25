// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Components/TimelineComponent.h"
#include "MyStaticMeshActor.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT2_API AMyStaticMeshActor : public AStaticMeshActor
{
	GENERATED_BODY()

	class UTimelineComponent* MyTimeline;
	
public:
	AMyStaticMeshActor();

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Timeline")
	class UCurveFloat* MyCurve;

	UPROPERTY()
		FVector startLocation;

	UPROPERTY()
		FVector endLocation;

	UPROPERTY(EditAnywhere, Category = "Timeline")
		float ZOffset;

	FOnTimelineFloat InterpFunction{};

	FOnTimelineEvent TimelineFinished{};

	UFUNCTION()
		void TimelineFloatReturn(float value);
	UFUNCTION()
		void OnTimelineFinished();
};
