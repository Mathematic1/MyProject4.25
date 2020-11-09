// Fill out your copyright notice in the Description page of Project Settings.


#include "SplineActor.h"
#include "Components/SplineMeshComponent.h"

// Sets default values
ASplineActor::ASplineActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	if (SplineComponent) {
		SetRootComponent(SplineComponent);
	}

}

void ASplineActor::OnConstruction(const FTransform & Transform)
{
	if (SplineMeshMap.Num() > 0) {
		/*if (!Mesh) {
			return;
		}*/
		FSplineMeshDetails* DefaultMeshDetails = nullptr;
		if (SplineMeshMap.Contains(ESplineMeshType::DEFAULT)) {
			DefaultMeshDetails = SplineMeshMap.Find(ESplineMeshType::DEFAULT);
		}
		else {
			return;
		}

		FSplineMeshDetails* StartMeshDetails = nullptr;
		if (SplineMeshMap.Contains(ESplineMeshType::START)) {
			StartMeshDetails = SplineMeshMap.Find(ESplineMeshType::START);
		}

		FSplineMeshDetails* EndMeshDetails = nullptr;
		if (SplineMeshMap.Contains(ESplineMeshType::END)) {
			EndMeshDetails = SplineMeshMap.Find(ESplineMeshType::END);
		}

		for (int SplineCount = 0; SplineCount < (SplineComponent->GetNumberOfSplinePoints() - 1); SplineCount++) {
			USplineMeshComponent* SplineMeshComponent = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());

			SplineMeshComponent->SetMobility(EComponentMobility::Movable);
			SplineMeshComponent->CreationMethod = EComponentCreationMethod::UserConstructionScript;
			SplineMeshComponent->RegisterComponentWithWorld(GetWorld());
			SplineMeshComponent->AttachToComponent(SplineComponent, FAttachmentTransformRules::KeepRelativeTransform);

			const FVector StartPoint = SplineComponent->GetLocationAtSplinePoint(SplineCount, ESplineCoordinateSpace::Local);
			const FVector StartTangent = SplineComponent->GetTangentAtSplinePoint(SplineCount, ESplineCoordinateSpace::Local);
			const FVector EndPoint = SplineComponent->GetLocationAtSplinePoint(SplineCount + 1, ESplineCoordinateSpace::Local);
			const FVector EndTangent = SplineComponent->GetTangentAtSplinePoint(SplineCount + 1, ESplineCoordinateSpace::Local);

			SplineMeshComponent->SetStartAndEnd(StartPoint, StartTangent, EndPoint, EndTangent, true);

			SplineMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

			//SplineMeshComponent->SetForwardAxis(ForwardAxis);

			/*if (StartMesh&&SplineCount == 0) {

				SplineMeshComponent->SetStaticMesh(StartMesh);
			}
			else if (EndMesh&&SplineComponent->GetNumberOfSplinePoints() > 2 && SplineCount == (SplineComponent->GetNumberOfSplinePoints() - 2)) {

				SplineMeshComponent->SetStaticMesh(EndMesh);
			}
			else {

				SplineMeshComponent->SetStaticMesh(Mesh);

				if (AlternativeMaterial&&SplineCount > 0 && SplineCount % 2 == 0) {
					SplineMeshComponent->SetMaterial(0, AlternativeMaterial);
				}
				else if (DefaultMaterial) {
					SplineMeshComponent->SetMaterial(0, DefaultMaterial);
				}
			}*/
			if (StartMeshDetails&&StartMeshDetails->Mesh&&SplineCount == 0) {
				SplineMeshComponent->SetStaticMesh(StartMeshDetails->Mesh);
				SplineMeshComponent->SetForwardAxis(StartMeshDetails->ForwardAxis);
			}
			else if (EndMeshDetails&&EndMeshDetails->Mesh&&SplineComponent->GetNumberOfSplinePoints() > 2 && SplineCount == (SplineComponent->GetNumberOfSplinePoints() - 2)) {
				SplineMeshComponent->SetStaticMesh(EndMeshDetails->Mesh);
				SplineMeshComponent->SetForwardAxis(EndMeshDetails->ForwardAxis);
			}
			else {
				if (DefaultMeshDetails->Mesh) {
					SplineMeshComponent->SetStaticMesh(DefaultMeshDetails->Mesh);
					SplineMeshComponent->SetForwardAxis(DefaultMeshDetails->ForwardAxis);
					if (DefaultMeshDetails->AlternativeMaterial&&SplineCount > 0 && SplineCount % 2 == 0) {
						SplineMeshComponent->SetMaterial(0, DefaultMeshDetails->AlternativeMaterial);
					}
					else if (DefaultMeshDetails->DefaultMaterial) {
						SplineMeshComponent->SetMaterial(0, DefaultMeshDetails->DefaultMaterial);
					}
				}
			}
		}
	}
}

// Called when the game starts or when spawned
void ASplineActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASplineActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

