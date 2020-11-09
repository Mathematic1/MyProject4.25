// Fill out your copyright notice in the Description page of Project Settings.


#include "MyStaticMeshActor.h"
#include "Runtime/Engine/Classes/Components/TimelineComponent.h"

AMyStaticMeshActor::AMyStaticMeshActor()
{
	PrimaryActorTick.bCanEverTick = true;
	MyTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Timeline"));
	InterpFunction.BindUFunction(this, FName("TimelineFloatReturn"));
	TimelineFinished.BindUFunction(this, FName("OnTimelineFinished"));
	ZOffset = 50.0f;
}

void AMyStaticMeshActor::BeginPlay()
{
	Super::BeginPlay();

	if (MyCurve) {
		MyTimeline->AddInterpFloat(MyCurve, InterpFunction, FName("Alpha"));
		MyTimeline->SetTimelineFinishedFunc(TimelineFinished);

		startLocation = GetActorLocation();
		endLocation = FVector(startLocation.X, startLocation.Y, startLocation.Z + ZOffset);

		MyTimeline->SetLooping(false);
		MyTimeline->SetIgnoreTimeDilation(true);

		MyTimeline->Play();
	}
}

void AMyStaticMeshActor::TimelineFloatReturn(float value)
{
	SetActorLocation(FMath::Lerp(startLocation, endLocation, value));
}

void AMyStaticMeshActor::OnTimelineFinished()
{
	if (MyTimeline->GetPlaybackPosition() == 0.0f) {
		GLog->Log("Play");
		MyTimeline->Play();
	}
	else if(MyTimeline->GetPlaybackPosition()==MyTimeline->GetTimelineLength()){
		GLog->Log("Reverse");
		MyTimeline->Reverse();
	}
}

