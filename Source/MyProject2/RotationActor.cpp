// Fill out your copyright notice in the Description page of Project Settings.


#include "RotationActor.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/Public//TimerManager.h"
#include "BatteryMan.h"

// Sets default values
ARotationActor::ARotationActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RotationInTick = FRotator(0.0f, 1.0f, 0.0f);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh123"));
	RootComponent = Mesh;
	Mesh->SetCollisionProfileName(TEXT("BlockAll"));
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionObjectType(ECC_WorldStatic);
	Mesh->SetRelativeScale3D(FVector(5.f, 5.f, 0.5f));
	boxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	boxCollision->SetupAttachment(Mesh);
	boxCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	MeshCollision = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshCollision"));
	MeshCollision->SetupAttachment(Mesh);
	MeshCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

}

FRotator ARotationActor::GetRotation()
{
	return GetActorRotation();
}



void ARotationActor::OnBeginOverlap(UPrimitiveComponent * HitComponent, AActor * OtherActor, UPrimitiveComponent * OtherComponent, int32 OtherBodyIndex, bool bFroomSweep, const FHitResult & SweepResult)
{
	if (OtherActor->ActorHasTag("Player")) {
		Mesh->SetMaterial(0, matInterface[FMath::RandRange(1, 3)]);
		directionOfPlatform = GetActorUpVector();
		GetWorldTimerManager().SetTimer(timer, this, &ARotationActor::upPlatform, GetWorld()->GetDeltaSeconds(), true, 0.f);
		GetActorLocation() += GetActorUpVector();
	}
}

void ARotationActor::OnEndOverlap(UPrimitiveComponent * HitComponent, AActor * OtherActor, UPrimitiveComponent * OtherComponent, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag("Player")) {
		Mesh->SetMaterial(0, matInterface[0]);
	}

	FHitResult hit;
	FVector startLocation = GetActorLocation();
	FVector finishLocation = OtherActor->GetActorLocation();
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByChannel(hit, startLocation, finishLocation, ECollisionChannel::ECC_Visibility, queryParams);
	UKismetSystemLibrary::DrawDebugLine(GetWorld(), startLocation, finishLocation, FColor::Blue, 5.f, 3.f);
	UE_LOG(LogTemp, Warning, TEXT("Character name is %s"), *hit.GetActor()->GetFName().ToString());
	//GetWorldTimerManager().ClearTimer(timer);
	directionOfPlatform = GetActorUpVector().DownVector;

	/*if ([=]() {return true;}()) {

	}*/
}

void ARotationActor::OnPlace(AActor * OverllapedActor, AActor * OtherActor)
{
	if (OtherActor->IsA(ABatteryMan::StaticClass())) {
		UE_LOG(LogTemp, Warning, TEXT("159753"))
	}
}

void ARotationActor::SetActorRotate(FRotator ActorRotation)
{
	SetActorRotation(ActorRotation + RotationInTick);
}

void ARotationActor::upPlatform()
{
	if (GetActorLocation().Z > 165||directionOfPlatform==GetActorUpVector()) {
		FVector location = GetActorLocation() + directionOfPlatform;
		SetActorLocation(location);
	}
}

// Called when the game starts or when spawned
void ARotationActor::BeginPlay()
{
	Super::BeginPlay();

	MeshCollision->OnComponentBeginOverlap.AddDynamic(this, &ARotationActor::OnBeginOverlap);
	MeshCollision->OnComponentEndOverlap.AddDynamic(this, &ARotationActor::OnEndOverlap);

	MeshCollision->SetStaticMesh(Mesh->GetStaticMesh());
	MeshCollision->SetHiddenInGame(true);

	OnActorBeginOverlap.AddDynamic(this, &ARotationActor::OnPlace);
	
}

// Called every frame
void ARotationActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetActorRotate(GetRotation());

}

