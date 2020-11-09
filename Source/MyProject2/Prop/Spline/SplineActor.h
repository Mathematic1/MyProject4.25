// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "SplineActor.generated.h"

UENUM(BlueprintType)
enum class ESplineMeshType : uint8 {
	DEFAULT		UMETA(DisplayName="Default mesh"),
	START		UMETA(DisplayName="Starting mesh"),
	END			UMETA(DisplayName="End mesh")
};

USTRUCT(BlueprintType)
struct FSplineMeshDetails :public FTableRowBase {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TEnumAsByte<ESplineMeshAxis::Type> ForwardAxis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UMaterialInterface* DefaultMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UMaterialInterface* AlternativeMaterial;

	FSplineMeshDetails() :ForwardAxis(ESplineMeshAxis::X) {

	}
};

UCLASS()
class MYPROJECT2_API ASplineActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASplineActor();

	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category = "Spline")
	USplineComponent* SplineComponent;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline")
	UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline")
	UStaticMesh* StartMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline")
	UStaticMesh* EndMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline")
	TEnumAsByte<ESplineMeshAxis::Type> ForwardAxis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline")
		class UMaterialInterface* DefaultMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline")
		class UMaterialInterface* AlternativeMaterial;*/

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spline")
	TMap<ESplineMeshType, FSplineMeshDetails> SplineMeshMap;
};
