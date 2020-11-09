// Fill out your copyright notice in the Description page of Project Settings.


#include "BatteryMan.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "RotationActor.h"
#include "Battery.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"
#include "Components/SceneComponent.h"
#include "Sound/SoundCue.h"
#include "Engine/Public/EngineUtils.h"
#include "Engine/Public/TimerManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Gameframework/SpringArmComponent.h"
#include "UI/InGameHUD.h"
#include "Prop/InteractiveProp.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "GameFramework/PlayerInput.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "BatteryManMPProjectile.h"
#include "ProjectPlayerController.h"
#include "ProjectPlayerState.h"
#include "BatteryMan_GameMode.h"
#include "PlayerAnimInstance.h"

FVector VectorUp =  FVector(0.0f, 0.0f, 250.0f);
TTuple<float, int32> rotActorParams;

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);
// Sets default values
ABatteryMan::ABatteryMan()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 0.2f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	ProjectileClass = ABatteryManMPProjectile::StaticClass();
	FireRate = 0.25f;
	bIsFiringWeapon = false;

	bDead = false;
	Power = 100.0f;
	bRollFalling = false;

	VectorUp2 = this->GetActorUpVector() * 500;

	left_MeleeCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("left_MeleeCollision"));
	left_MeleeCollision->SetupAttachment(RootComponent);
	left_MeleeCollision->SetCollisionProfileName("NoCollision");
	left_MeleeCollision->SetNotifyRigidBodyCollision(false);

	right_MeleeCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("right_MeleeCollision"));
	right_MeleeCollision->SetupAttachment(RootComponent);
	right_MeleeCollision->SetCollisionProfileName("NoCollision");
	right_MeleeCollision->SetNotifyRigidBodyCollision(false);

	LineTraceType = ELineTraceType::CAMERA_SINGLE;
	LineTraceDistance = 100.f;
	LineTraceSpread = 10.f;

	CurrentComboCount = 0;

	MaxCountdownToIdle = 15;

	MaxCrouchSpeed = 200.f;
	MaxWalkSpeed = 250.0f;
	MaxRunSpeed = 600.0f;
	MaxArmedSpeed = 200.0f;

	MaxHealth = 100.f;
	CurrentHealth = MaxHealth;

	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = MaxCrouchSpeed;

	AttackStrength = EAttackStrength::MEDIUM;

	bIsKeyboardEnabled = true;
	bIsCurrentlyPossessed = false;
	RollIndex = 0;
	AllComboCount = 0;
	bIsChangeableRotation = true;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> MaleeAttackMontageObject(TEXT("AnimMontage'/Game/PunchMontage.PunchMontage'")); 
	if (MaleeAttackMontageObject.Succeeded())
		animMontage = MaleeAttackMontageObject.Object;
	static ConstructorHelpers::FObjectFinder<UDataTable> dataAttackObject(TEXT("DataTable'/Game/Blueprints/AttackDataTable.AttackDataTable'"));
	if (dataAttackObject.Succeeded())
		attackData = dataAttackObject.Object;
	static ConstructorHelpers::FObjectFinder<UDataTable> PlayerAttackDataTableObject(TEXT("DataTable'/Game/Blueprints/PlayerAttackDataTable.PlayerAttackDataTable'"));
	if (PlayerAttackDataTableObject.Succeeded())
		PlayerAttackDataTable = PlayerAttackDataTableObject.Object;
	static ConstructorHelpers::FObjectFinder<USoundCue> punchSoundObject(TEXT("SoundCue'/Game/Sounds/PunchCueA.PunchCueA'"));
	if (punchSoundObject.Succeeded()) {
		punchSound = punchSoundObject.Object;
		punchAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("PunchAudioComponent"));
		punchAudioComponent->SetupAttachment(RootComponent);
	}
	static ConstructorHelpers::FObjectFinder<USoundCue> punchSoundThrowObject(TEXT("SoundCue'/Game/Sounds/PunchCueB.PunchCueB'"));
	if (punchSoundThrowObject.Succeeded()) {
		punchThrowSound = punchSoundThrowObject.Object;
		punchAudioThrowComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("PunchAudioThrowComponent"));
		punchAudioThrowComponent->SetupAttachment(RootComponent);
	}
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MeleeFistAttackMontageObject(TEXT("AnimMontage'/Game/Animation/Punch/MeleeFistAttackMontage2.MeleeFistAttackMontage2'"));
	if (MeleeFistAttackMontageObject.Succeeded()) {
		MeleeFistAttackMontage = MeleeFistAttackMontageObject.Object;
	}
}

// Called when the game starts or when spawned
void ABatteryMan::BeginPlay()
{
	Super::BeginPlay();

	UWidgetBlueprintLibrary::SetInputMode_GameOnly(GetWorld()->GetFirstPlayerController());
	
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ABatteryMan::OnBeginOverlap);

	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &ABatteryMan::EndOverlap);

	left_MeleeCollision->OnComponentHit.AddDynamic(this, &ABatteryMan::OnAttackHit);
	right_MeleeCollision->OnComponentHit.AddDynamic(this, &ABatteryMan::OnAttackHit);

	//GameMode = GetWorld()->GetAuthGameMode<ABatteryMan_GameMode>();

	if (attackData) {
		FPlayerAttackMontage attackMontage;
		attackMontage.AnimMontage = NULL;
		attackMontage.indexOfSection = 10;
		attackMontage.Description = "Description for montage";
		attackData->AddRow(FName(TEXT("Punch10")), attackMontage);
	}
	if (punchSound&&punchAudioComponent) {
		punchAudioComponent->SetPitchMultiplier(FMath::RandRange(1.f, 1.3f));
		punchAudioComponent->SetSound(punchSound);
		//punchAudioComponent->Play(0.f);
	}
	if (punchThrowSound&&punchAudioThrowComponent) {
		punchAudioThrowComponent->SetPitchMultiplier(FMath::RandRange(1.f, 1.3f));
		punchAudioThrowComponent->SetSound(punchThrowSound);
	}
}

void ABatteryMan::EndPlay(EEndPlayReason::Type Reason)
{
	UE_LOG(LogTemp, Warning, TEXT("EndPlay"));
}

void ABatteryMan::EndPlay1()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), 0, EQuitPreference::Quit, false);
}

void ABatteryMan::TimeSlow()
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), timeDilation);
}

void ABatteryMan::TimeNormal()
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);
	GetWorldTimerManager().ClearTimer(Timer1);
}

/*void ABatteryMan::AttackLeft()
{
	if (attackData) {
		static const FString ContextString(TEXT("PlayerAttackMontageContext"));
		AttackMontage = attackData->FindRow<FPlayerAttackMontage>(FName(TEXT("Punch1")), ContextString, true);
		if (AttackMontage) {
			PlayAnimMontage(AttackMontage->AnimMontage, AnimationVarible, FName("Attack" + FString::FromInt(AttackMontage->indexOfSection)));
		}
	}
}

void ABatteryMan::AttackRight()
{
	if (attackData) {
		static const FString ContextString(TEXT("PlayerAttackMontageContext"));
		AttackMontage = attackData->FindRow<FPlayerAttackMontage>(FName(TEXT("Punch2")), ContextString, true);
		if (AttackMontage) {
			PlayAnimMontage(AttackMontage->AnimMontage, AnimationVarible, FName("Attack" + FString::FromInt(AttackMontage->indexOfSection)));
		}
	}
}*/

void ABatteryMan::OnAttackHit(UPrimitiveComponent * HitComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{
	log(ELogLevel::INFO, Hit.GetActor()->GetName(), ELogOutput::ALL);
	if (!bDead) {
		if (!HasAuthority()) {
			if (punchAudioComponent && !punchAudioComponent->IsPlaying())
			{
				if (!punchAudioComponent->IsActive())
				{
					punchAudioComponent->Activate(true);
				}
				punchAudioComponent->SetPitchMultiplier(FMath::FRandRange(1.0f, 1.3f));
				punchAudioComponent->Play(0.f);
			}
		}
		UAnimInstance * AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance) {
			if (!AnimInstance->Montage_IsPlaying(AttackMontageEdit->AnimMontage))
				AnimInstance->Montage_Pause(AttackMontageEdit->AnimMontage);
			else
				AnimInstance->Montage_Play(AttackMontageEdit->AnimMontage, (float)-AnimationVarible/2, EMontagePlayReturnType::Duration, AnimInstance->Montage_GetPosition(AttackMontageEdit->AnimMontage), true);
				//AnimInstance->Montage_Stop(AnimationVarible, AttackMontage->AnimMontage);
			//AnimInstance->Montage_Pause(AttackMontageEdit->AnimMontage);
		}

			/*if (InGameHUD) {
				CurrentComboCount += 1;
				InGameHUD->UpdateComboCount(CurrentComboCount);
			}*/

		if (IsLocallyControlled()) {
			ComboServer(CurrentComboCount);
		}

		/*if (!GetWorld()->GetTimerManager().IsTimerActive(ComboResetHandle)) {
			GetWorld()->GetTimerManager().SetTimer(ComboResetHandle, this, &ABatteryMan::ResetCombo, 5.f, false);
		}
		else {
			GetWorld()->GetTimerManager().ClearTimer(ComboResetHandle);
			GetWorld()->GetTimerManager().SetTimer(ComboResetHandle, this, &ABatteryMan::ResetCombo, 5.f, false);
		}*/
	}
}

void ABatteryMan::ResetCombo()
{
	CurrentComboCount = 0;
	//AInGameHUD* InGameHUD = Cast<AInGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	if (InGameHUD) {
		InGameHUD->ResetCombo();
	}
}

void ABatteryMan::CrouchStart()
{
	CrouchStartServer();
	/*if (bIsArmed) {
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;

		GetCharacterMovement()->MaxWalkSpeedCrouched = MaxArmedSpeed;
	}
	else {
		GetCharacterMovement()->MaxWalkSpeedCrouched = MaxCrouchSpeed;
	}*/

	Crouch();
}

void ABatteryMan::CrouchEnd()
{
	CrouchEndServer();
	/*if (bIsArmed) {
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;

		GetCharacterMovement()->MaxWalkSpeedCrouched = MaxArmedSpeed;
	}
	else {
		if (bIsRunning) {
			GetCharacterMovement()->MaxWalkSpeedCrouched = MaxRunSpeed;
		}
		else {
			GetCharacterMovement()->MaxWalkSpeedCrouched = MaxWalkSpeed;
		}
	}*/

	UnCrouch();
}

void ABatteryMan::ArmPlayer(bool Value)
{
	bIsArmed = Value;
	/*log(ELogLevel::WARNING, __FUNCTION__, ELogOutput::ALL);
	if (!bIsArmed) {
		CountdownToIdle = MaxCountdownToIdle;
		GetWorld()->GetTimerManager().ClearTimer(ArmedToIdleTimerHandle);

		if (bIsRunning) {
			GetCharacterMovement()->MaxWalkSpeed = MaxRunSpeed;
		}
		else {
			GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
		}
	}
	else {
		GetCharacterMovement()->MaxWalkSpeed = MaxArmedSpeed;
	}*/
}

void ABatteryMan::ArmPlayerImmediately()
{
	SetArmedServer(!bIsArmed);
}

void ABatteryMan::RunStart()
{
	SetIsRunningServer(true);
}

void ABatteryMan::RunEnd()
{
	SetIsRunningServer(false);
}

void ABatteryMan::TriggerCountdownToIdle()
{
	log(ELogLevel::WARNING, FString::FromInt(CountdownToIdle), ELogOutput::ALL);

	if (--CountdownToIdle <= 0) {
		if (!HasAuthority()) {
			SetArmedServer(false);
		}
		CountdownToIdle = MaxCountdownToIdle;
		GetWorld()->GetTimerManager().ClearTimer(ArmedToIdleTimerHandle);
	}
}

void ABatteryMan::Interact()
{
	FVector start;
	FVector end;

	FVector PlayerEyesLoc;
	FRotator PlayerEyesRot;

	GetActorEyesViewPoint(PlayerEyesLoc, PlayerEyesRot);

	start = PlayerEyesLoc;
	end = PlayerEyesLoc + (PlayerEyesRot.Vector() * LineTraceDistance);

	FCollisionQueryParams TraceParams(FName(TEXT("InteractTrace")), true, this);

	FHitResult InteractHit = FHitResult(ForceInit);

	bool bIsHit = GetWorld()->LineTraceSingleByChannel(InteractHit, start, end, ECC_GameTraceChannel1, TraceParams);

	if (bIsHit&&InteractHit.GetActor()!=this) {
		log(ELogLevel::WARNING, InteractHit.Actor->GetName(), ELogOutput::ALL);
		DrawDebugLine(GetWorld(), start, end, FColor::Blue, false, 5.f, ECC_WorldStatic, 1.f);
		/*if (InteractHit.GetActor()->GetClass()->ImplementsInterface(UInterractiveActor::StaticClass())) {
			IInterractiveActor::Execute_Interact(InteractHit.GetActor());
		}*/
		/*if (InteractHit.GetActor()->IsA(UInterractiveActor::StaticClass())) {
			IInterractiveActor::Execute_Interact(InteractHit.GetActor());
		}*/
		IInterractiveActor* InteractiveActor = Cast<IInterractiveActor>(InteractHit.GetActor());
		if (InteractiveActor) {
			IInterractiveActor::Execute_Interact(InteractHit.GetActor());
		}
		else if (InteractHit.GetActor()->IsA(ABatteryMan::StaticClass())) {
			if (bIsCurrentlyPossessed) {
				bIsCurrentlyPossessed = false;
				if (DefaultMaterialBody) {
					GetMesh()->SetMaterial(0, DefaultMaterialBody);
				}
				if (DefaultMaterialChest) {
					GetMesh()->SetMaterial(1, DefaultMaterialChest);
				}
			}
			ABatteryMan* PossessableCharacter = Cast<ABatteryMan>(InteractHit.GetActor());
			if (PossessableCharacter&&!PossessableCharacter->bIsCurrentlyPossessed) {
				if (!SavedController) {
					SavedController = GetController();
				}

				SavedController->UnPossess();

				bIsKeyboardEnabled = false;
				bIsRunning = false;
				if (!HasAuthority()) {
					SetArmedServer(false);
				}

				GetCharacterMovement()->SetMovementMode(MOVE_None);

				SavedController->Possess(PossessableCharacter);

				PossessableCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);

				if (PossessableCharacter->PossessedMaterialBody) {
					PossessableCharacter->GetMesh()->SetMaterial(0, PossessableCharacter->PossessedMaterialBody);
				}
				if (PossessableCharacter->PossessedMaterialChest) {
					PossessableCharacter->GetMesh()->SetMaterial(1, PossessableCharacter->PossessedMaterialChest);
				}

				PossessableCharacter->bIsCurrentlyPossessed = true;
				PossessableCharacter-> bIsKeyboardEnabled = true;
			}
		}
	}
	else {
		DrawDebugLine(GetWorld(), start, end, FColor::Green, false, 5.f, ECC_WorldStatic, 1.f);
	}
}

void ABatteryMan::OnHealthUpdate()
{
	if (IsLocallyControlled()) {
		FString healthMessage = FString::Printf(TEXT("You now have %f health remainig"), CurrentHealth);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);

		if (CurrentHealth <= 0)
		{
			FString deathMessage = FString::Printf(TEXT("You have been killed."));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, deathMessage);
		}
	}

	if (GetLocalRole() == ROLE_Authority)
	{
		FString healthMessage = FString::Printf(TEXT("%s now has %f health remaining."), *GetFName().ToString(), CurrentHealth);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);
	}
}

void ABatteryMan::StartFire()
{
	if (!bIsFiringWeapon)
	{
		bIsFiringWeapon = true;
		UWorld* World = GetWorld();
		World->GetTimerManager().SetTimer(FiringTimer, this, &ABatteryMan::StopFire, FireRate, false);
		HandleFire();
	}
}

void ABatteryMan::StopFire()
{
	bIsFiringWeapon = false;
}

void ABatteryMan::HandleFire_Implementation()
{
	FVector spawnLocation = GetActorLocation() + (GetControlRotation().Vector()  * 100.0f) + (GetActorUpVector() * 50.0f);
	FRotator spawnRotation = GetControlRotation();

	FActorSpawnParameters spawnParameters;
	spawnParameters.Instigator = GetInstigator();
	spawnParameters.Owner = this;

	ABatteryManMPProjectile* spawnedProjectile = GetWorld()->SpawnActor<ABatteryManMPProjectile>(spawnLocation, spawnRotation, spawnParameters);
}


void ABatteryMan::AttackStart()
{
	left_MeleeCollision->SetCollisionProfileName("Weapon");
	left_MeleeCollision->SetNotifyRigidBodyCollision(true);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *left_MeleeCollision->GetCollisionProfileName().ToString());

	right_MeleeCollision->SetCollisionProfileName("Weapon");
	right_MeleeCollision->SetNotifyRigidBodyCollision(true);

	//ArmPlayer(true);
}

void ABatteryMan::AttackEnd()
{
	log(ELogLevel::INFO, __FUNCTION__, ELogOutput::ALL);
	left_MeleeCollision->SetCollisionProfileName("NoCollision");
	left_MeleeCollision->SetNotifyRigidBodyCollision(false);

	right_MeleeCollision->SetCollisionProfileName("NoCollision");
	right_MeleeCollision->SetNotifyRigidBodyCollision(false);



	/*bool bIsCountdownActive = GetWorld()->GetTimerManager().IsTimerActive(ArmedToIdleTimerHandle);
	if (bIsCountdownActive) {
		GetWorld()->GetTimerManager().ClearTimer(ArmedToIdleTimerHandle);
	}

	CountdownToIdle = MaxCountdownToIdle;
	GetWorld()->GetTimerManager().SetTimer(ArmedToIdleTimerHandle, this, &ABatteryMan::TriggerCountdownToIdle, 1.f, true);*/

}

void ABatteryMan::FinishAttack()
{
	UAnimInstance * AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance) {
		AnimInstance->Montage_Stop(0.2f, AttackMontage->AnimMontage);
	}
}

void ABatteryMan::LightAttackStart()
{
	SetLightAttackServer(true);
}

void ABatteryMan::LightAttackEnd()
{
	SetLightAttackServer(false);
}

void ABatteryMan::StrongAttackStart()
{
	SetStrongAttackServer(true);
}

void ABatteryMan::StrongAttackEnd()
{
	SetStrongAttackServer(false);
}

bool ABatteryMan::GetIsAnimationBlended()
{
	return IsAnimationBlended;
}

void ABatteryMan::StopRollMontage()
{
	if (HasAuthority()) {
		UAnimInstance * AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Stop(AnimationVarible, RollMontage);
		RollIndex = 0;
		bIsRoll = false;
		bIsKeyboardEnabled = true;
		StopMontage();
	}
}

bool ABatteryMan::IsArmed()
{
	return bIsArmed;
}

bool ABatteryMan::IsRunning()
{
	return bIsRunning;
}

float ABatteryMan::GetMoveRight()
{
	return MoveRightValue;
}

float ABatteryMan::GetMoveForward()
{
	return MoveForwardValue;
}

void ABatteryMan::SetIsKeyboardEnabled(bool Enabled)
{
	bIsKeyboardEnabled = Enabled;
}

bool ABatteryMan::GetAnimRoll()
{
	return bIsRoll;
}

void ABatteryMan::SetAnimRoll(bool isRoll)
{
	bIsRoll = isRoll;
}

EAttackType ABatteryMan::GetCurrentAttack()
{
	return CurrentAttack;
}

void ABatteryMan::SetCurrentHealth(float healthValue)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		CurrentHealth = FMath::Clamp(healthValue, 0.f, MaxHealth);
		OnHealthUpdate();
	}
}

float ABatteryMan::TakeDamage(float DamageTaken, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	float damageApplied = CurrentHealth - DamageTaken;
	SetCurrentHealth(damageApplied);
	return damageApplied;
}

bool ABatteryMan::ServerKick_Validation()
{
	return true;
}

void ABatteryMan::ServerKick_Implementation()
{
	if (HasAuthority()) {
		AttackInput(EAttackType::MELEE_KICK);
		ClientKick();
		ArmPlayer(true);
		bIsKeyboardEnabled = false;
		IsAnimationBlended = false;
	}
}

bool ABatteryMan::ClientKick_Validation()
{
	return true;
}

void ABatteryMan::ClientKick_Implementation()
{
	if (!HasAuthority()) {
		AttackInput(EAttackType::MELEE_KICK);
	}
}

void ABatteryMan::ServerFist_Implementation()
{
	if (HasAuthority()) {
		AttackInput(EAttackType::MELEE_FIST);
		MultiFist();
		ArmPlayer(true);
		bIsKeyboardEnabled = true;
		IsAnimationBlended = true;
	}
}

void ABatteryMan::MultiFist_Implementation()
{
	if (!HasAuthority()) {
		AttackInput(EAttackType::MELEE_FIST);
	}
}

void ABatteryMan::StopMontage_Implementation()
{
	UAnimInstance * AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Stop(AnimationVarible, RollMontage);
}

void ABatteryMan::SetIsRunningServer_Implementation(bool bIsRun)
{
	if (HasAuthority()) {
		if (bIsRun) {
			if (!bIsArmed && !bIsCrouched) {
				bIsRunning = true;
				//GetCharacterMovement()->MaxWalkSpeed = MaxRunSpeed;
			}
		}
		else {
			if (!bIsArmed && !bIsCrouched) {
				bIsRunning = false;
				//GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
			}
		}
	}
}

void ABatteryMan::SetArmedServer_Implementation(bool bIsArm)
{
	if (HasAuthority()) {
		ArmPlayer(bIsArm);
	}
}

void ABatteryMan::SetArmedMulti_Implementation(bool bIsArm)
{
	ArmPlayer(bIsArm);
}

void ABatteryMan::SetStrongAttackServer_Implementation(bool bIsStrong)
{
	bIsAttackStrong = bIsStrong;
}

void ABatteryMan::SetLightAttackServer_Implementation(bool bIsLight)
{
	bIsAttackLight = bIsLight;
}

void ABatteryMan::RollServer_Implementation()
{
	if (HasAuthority()) {
		if (IsRunning()) {
			RollIndex = FMath::RandRange(4, 5);
		}
		else if (GetCharacterMovement()->IsFalling()) {
			RollIndex = 3;
		}
		else {
			RollIndex = FMath::RandRange(1, 2);
		}
		RollInput();
		bIsRoll = true;
	}
}

void ABatteryMan::ComboServer_Implementation(int32 ComboCount)
{
	if (HasAuthority()) {
		AProjectGameState* GS = Cast<AProjectGameState>(UGameplayStatics::GetGameState(GetWorld()));
		if (GS) {
			GS->AllComboCount++;
			AllComboCount = GS->AllComboCount;
		}
	}
	ComboCount++;
	CurrentComboCount = ComboCount;
}

void ABatteryMan::MoveForwardServer_Implementation(float Axis)
{
	//MoveForwardValue = Axis;
	if (Axis == 0.0f) {
		bInputMoveFwd = false;
		bInputMoveBack = false;
	}
	if (Controller != NULL && !bDead&&bIsKeyboardEnabled && !bIsRoll && (!Axis == 0.0f)) {
		if (Axis > 0.0f) {
			bInputMoveFwd = true;
			bInputMoveBack = false;
		}
		else if (Axis < 0.0f) {
			bInputMoveFwd = false;
			bInputMoveBack = true;
		}
		/*if (bIsArmed && !bIsCrouched) {
			if (!GetCharacterMovement()->bUseControllerDesiredRotation) {
				GetCharacterMovement()->bUseControllerDesiredRotation = true;
				GetCharacterMovement()->bOrientRotationToMovement = false;
			}
		}
		else {
			if (GetCharacterMovement()->bUseControllerDesiredRotation) {
				GetCharacterMovement()->bUseControllerDesiredRotation = false;
				GetCharacterMovement()->bOrientRotationToMovement = true;
			}
		}*/
	}
}

void ABatteryMan::MoveRightServer_Implementation(float Axis)
{
	//MoveRightValue = Axis;
	if (Axis == 0.0f) {
		bInputMoveLeft = false;
		bInputMoveRight = false;
	}
	if (Controller != NULL && !bDead&&bIsKeyboardEnabled && !bIsRoll && (!Axis == 0.0f)) {
		if (Axis > 0.0f) {
			bInputMoveLeft = false;
			bInputMoveRight = true;
		}
		else if (Axis < 0.0f) {
			bInputMoveLeft = true;
			bInputMoveRight = false;
		}
		/*if (bIsArmed && !bIsCrouched) {
			if (!GetCharacterMovement()->bUseControllerDesiredRotation) {
				GetCharacterMovement()->bUseControllerDesiredRotation = true;
				GetCharacterMovement()->bOrientRotationToMovement = false;
			}
		}
		else {
			if (GetCharacterMovement()->bUseControllerDesiredRotation) {
				GetCharacterMovement()->bUseControllerDesiredRotation = false;
				GetCharacterMovement()->bOrientRotationToMovement = true;
			}
		}*/
	}
}

void ABatteryMan::CrouchStartServer_Implementation()
{
	bIsCrouching = true;
	/*if (bIsArmed) {
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;

		GetCharacterMovement()->MaxWalkSpeedCrouched = MaxArmedSpeed;
	}
	else {
		GetCharacterMovement()->MaxWalkSpeedCrouched = MaxCrouchSpeed;
	}*/
}

void ABatteryMan::CrouchEndServer_Implementation()
{
	bIsCrouching = false;
	/*if (bIsArmed) {
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;

		GetCharacterMovement()->MaxWalkSpeedCrouched = MaxArmedSpeed;
	}
	else {
		if (bIsRunning) {
			GetCharacterMovement()->MaxWalkSpeedCrouched = MaxRunSpeed;
		}
		else {
			GetCharacterMovement()->MaxWalkSpeedCrouched = MaxWalkSpeed;
		}
	}*/
}

void ABatteryMan::SetCurrentControlRotation_Implementation(FRotator ControlRotator)
{
	ControlRotation = ControlRotator;
}

void ABatteryMan::ChangeOrientToMove_Implementation()
{
	bIsChangeableRotation = !bIsChangeableRotation;
}

void ABatteryMan::ChangeActorRotation_Implementation(FRotator ControllerRotation)
{
	SetActorRotation(FRotator(0.f, ControllerRotation.Yaw, 0.f));
}

// Called every frame
void ABatteryMan::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLocallyControlled()) {
		SetCurrentControlRotation(GetControlRotation());
		if (bIsChangeableRotation) {
			//ChangeActorRotation(GetControlRotation());
		}
	}
	UpdateInputState();
	/*Power -= DeltaTime * Power_Treshold;
//////////////////////////////////////////////////////////////////////////////////////////////
	if (Power <= -1000) {
		if (!bDead) {
			bDead = true;
			
			GetMesh()->SetSimulatePhysics(true);

			FTimerHandle UnusedHandle;
			GetWorldTimerManager().SetTimer(UnusedHandle, this, &ABatteryMan::RestartGame , 3.0f, false);
		}
	}*/

	if (StaticCast<int32>(EActiveState::Yes)== (int32)activeState) 
	{
		UE_LOG(LogTemp, Warning, TEXT("On platform"));
	}

	/*FHitResult hit;
	FVector startLocation = GetActorLocation();
	FVector finishLocation = GetActorLocation() + 4000.f*GetActorForwardVector();
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(this);
	const FVector directionVector = UKismetMathLibrary::GetDirectionUnitVector(startLocation, finishLocation);
	GetWorld()->LineTraceSingleByChannel(hit, startLocation, finishLocation, ECollisionChannel::ECC_Visibility, queryParams);
	UKismetSystemLibrary::DrawDebugLine(GetWorld(), startLocation, finishLocation, FColor::Red, 1.f, 3.f);
	startLocation = hit.Location;
	finishLocation = (hit.Location + 4000.f*FMath::GetReflectionVector(directionVector, hit.Normal));
	UKismetSystemLibrary::DrawDebugLine(GetWorld(), startLocation, finishLocation, FColor::Green, 1.f, 3.f);
	FCollisionQueryParams newQueryParams;
	newQueryParams.AddIgnoredActor(hit.GetActor());
	GetWorld()->LineTraceSingleByChannel(hit, startLocation, finishLocation, ECollisionChannel::ECC_Visibility, newQueryParams);*/
}

// Called to bind functionality to input
void ABatteryMan::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ABatteryMan::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABatteryMan::MoveRight);

	//PlayerInputComponent->BindAction("LeftPunch", IE_Pressed, this, &ABatteryMan::AttackLeft);
	//PlayerInputComponent->BindAction("RightPunch", IE_Pressed, this, &ABatteryMan::AttackRight);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ABatteryMan::CrouchStart);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ABatteryMan::CrouchEnd);

	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &ABatteryMan::RunStart);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &ABatteryMan::RunEnd);

	PlayerInputComponent->BindAction("ArmPlayer", IE_Pressed, this, &ABatteryMan::ArmPlayerImmediately);

	PlayerInputComponent->BindAction("Roll", IE_Pressed, this, &ABatteryMan::RollServer);

	PlayerInputComponent->BindAction("Punch", IE_Pressed, this, &ABatteryMan::PunchAttack);
	PlayerInputComponent->BindAction("Kick", IE_Released, this, &ABatteryMan::KickAttack);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ABatteryMan::StartFire);

	PlayerInputComponent->BindAction("LightAttackMod", IE_Pressed, this, &ABatteryMan::LightAttackStart);
	PlayerInputComponent->BindAction("LightAttackMod", IE_Released, this, &ABatteryMan::LightAttackEnd);

	PlayerInputComponent->BindAction("StrongAttackMod", IE_Pressed, this, &ABatteryMan::StrongAttackStart);
	PlayerInputComponent->BindAction("StrongAttackMod", IE_Released, this, &ABatteryMan::StrongAttackEnd);

	//PlayerInputComponent->BindAction("FireLineTrace", IE_Pressed, this, &ABatteryMan::FireLineTrace);
	PlayerInputComponent->BindAction("FireLineTrace", IE_Pressed, this, &ABatteryMan::Interact);
	PlayerInputComponent->BindAction("Show/HideInstruction", IE_Pressed, this, &ABatteryMan::ShowHideWidget);
	PlayerInputComponent->BindAction("RotationCamera", IE_Pressed, this, &ABatteryMan::ChangeOrientToMove);
}

void ABatteryMan::MoveForward(float Axis)
{
	MoveForwardServer(Axis);
	/*if (Controller != NULL && !bDead&&bIsKeyboardEnabled && !bIsRoll&& (!Axis == 0.0f)) {

		if (bIsArmed && !bIsCrouched) {
			if (!GetCharacterMovement()->bUseControllerDesiredRotation) {
				GetCharacterMovement()->bUseControllerDesiredRotation = true;
				GetCharacterMovement()->bOrientRotationToMovement = false;
			}

			//AddMovementInput(GetActorForwardVector(), MoveForwardValue);
		}
		else {
			if (GetCharacterMovement()->bUseControllerDesiredRotation) {
				GetCharacterMovement()->bUseControllerDesiredRotation = false;
				GetCharacterMovement()->bOrientRotationToMovement = true;
			}
			/*const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotator(0, Rotation.Yaw, 0);
			const FVector Direction = FRotationMatrix(YawRotator).GetUnitAxis(EAxis::X);
			AddMovementInput(Direction, Axis);

		}
	}*/
}

void ABatteryMan::MoveRight(float Axis)
{
	MoveRightServer(Axis);
	/*if (Controller != NULL && !bDead&&bIsKeyboardEnabled && !bIsRoll && (!Axis == 0.0f)) {
		if (bIsArmed && !bIsCrouched) {
			if (!GetCharacterMovement()->bUseControllerDesiredRotation) {
				GetCharacterMovement()->bUseControllerDesiredRotation = true;
				GetCharacterMovement()->bOrientRotationToMovement = false;
			}
			//AddMovementInput(GetActorRightVector(), MoveRightValue);
		}
		else {
			if (GetCharacterMovement()->bUseControllerDesiredRotation) {
				GetCharacterMovement()->bUseControllerDesiredRotation = false;
				GetCharacterMovement()->bOrientRotationToMovement = true;
			}

			/*const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotator(0, Rotation.Yaw, 0);
			const FVector Direction = FRotationMatrix(YawRotator).GetUnitAxis(EAxis::Y);
			AddMovementInput(Direction, Axis);
		}
	}*/
}

void ABatteryMan::OnBeginOverlap(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (!pickedItem.Battery) { pickedItem.Battery = Cast<ABattery>(OtherActor);
	if (pickedItem.Battery)pickedItem.RotActor = nullptr;}
	else { UE_LOG(LogTemp, Warning, TEXT("New element is battery")); }
	if (!pickedItem.RotActor) { pickedItem.RotActor = Cast<ARotationActor>(OtherActor);
	if (pickedItem.RotActor)pickedItem.Battery = nullptr;}
	else{ UE_LOG(LogTemp, Warning, TEXT("New element is rotation platform")); }
	if (OtherActor->ActorHasTag("Recharge")) {
		Power += 10.0f;

		if (Power > 100.0f)
			Power = 100.0f;

		OtherActor->Destroy();
	}

	if (OtherActor->ActorHasTag("Jumper")) {
	
		OtherActor->Destroy();

		latentInfo.CallbackTarget = this;

		myPosition = GetActorLocation();

		/*GetWorldTimerManager().SetTimerForNextTick([OtherActor, this]() { UKismetSystemLibrary::MoveComponentTo(GetRootComponent(),
			[=](){
			aactor = this;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), needClass, allActors);
			for (TActorIterator<ARotationActor> actorIt (GetWorld()); actorIt; ++actorIt) {
				ARotationActor* rtActor = *actorIt;
				aactor = rtActor;
			}
			UE_LOG(LogTemp, Warning, TEXT("111"));
			return aactor->GetActorLocation()+GetActorUpVector()*100;
		}(), GetActorRotation(), false, false, 1.f, false, EMoveComponentAction::Move, latentInfo); });*/


		//UKismetSystemLibrary::MoveComponentTo(GetRootComponent(), myPosition, GetActorRotation(), false, false, 1.f, false, EMoveComponentAction::Move,latentInfo);

		//GetWorldTimerManager().SetTimer(Timer, this, &ABatteryMan::EndPlay1, 3.f, false);
		GetWorldTimerManager().SetTimer(Timer1, this, &ABatteryMan::TimeNormal, 3.f*timeDilation, false);
		TimeSlow();
	}
	if (OtherActor->ActorHasTag("Platform"))
	{
		ARotationActor* rotActor = Cast<ARotationActor>(OtherActor);
		rotActorParams = rotationActorParams(rotActor);
		UE_LOG(LogTemp, Warning, TEXT("X: %f , Y: %i"), rotActorParams.Get<0>(), rotActorParams.Get<1>());
		activeState = EActiveState::Yes;
	}
}

void ABatteryMan::EndOverlap(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA(ARotationActor::StaticClass())) {
		activeState = EActiveState::No;
	}
}

void ABatteryMan::UpdateInputState()
{
	if (bInputMoveFwd || bInputMoveRight || bInputMoveLeft || bInputMoveBack) {
		bIsMoving = true;
	}
	else {
		bIsMoving = false;
	}
	InputMoveDirection = GetRelativeRotation();
	if (bIsMoving) {
		if (bIsRunning && !bIsArmed && !bIsCrouching) {
			MovingSpeed = 600.f;
		}
		else {
			MovingSpeed = 300.f;
		}
	}
	else {
		MovingSpeed = 0.0f;
	}
}

float ABatteryMan::GetRelativeRotation()
{
	DirectionMotion = ControlRotation.Yaw;
	MoveFwdBwdSum = (int32)bInputMoveFwd + (int32)bInputMoveBack *(-1);
	MoveRgtLftSum = (int32)bInputMoveRight + (int32)bInputMoveLeft*(-1);
	if (MoveFwdBwdSum != 0) {
		MoveRightDegreesMutiplier = 0.5f;
	}
	else {
		MoveRightDegreesMutiplier = 1.0f;
	}
	if (MoveFwdBwdSum == -1) {
		MoveRightDegreesMutiplier *= -1;
		DirectionMotion += 180;
	}
	if (MoveRgtLftSum == 1) {
		DirectionMotion = DirectionMotion + MoveRightDegreesMutiplier * 90.f;
	}
	else if (MoveRgtLftSum == -1) {
		DirectionMotion = DirectionMotion - MoveRightDegreesMutiplier * 90.f;
	}
	FRotator DirRotator = FRotator(0.0f, DirectionMotion, 0.0f);
	FRotator DeltaRotator = UKismetMathLibrary::NormalizedDeltaRotator(DirRotator, GetActorRotation());
	DirectionMotion = DeltaRotator.Yaw;
	return DirectionMotion;
}

TTuple<float, int32> ABatteryMan::rotationActorParams(ARotationActor * rotationActor)
{
	float x = rotationActor->Mesh->GetComponentScale().X;
	int32 y = rotationActor->MeshCollision->GetComponentScale().Y;
	return MakeTuple(x, y);
}

void ABatteryMan::ShowHideWidget()
{
	if (ProjectPC != nullptr) {
	}
}

void ABatteryMan::OnRep_CurrentHealth()
{
	AProjectPlayerState* PS = GetPlayerState<AProjectPlayerState>();
	if (PS) {
		PS->CurrentHealth = CurrentHealth;
	}
	OnHealthUpdate();
}

void ABatteryMan::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABatteryMan, CurrentHealth);
	DOREPLIFETIME(ABatteryMan, bIsArmed);
	DOREPLIFETIME(ABatteryMan, bIsAttackLight);
	DOREPLIFETIME(ABatteryMan, bIsAttackStrong);
	DOREPLIFETIME(ABatteryMan, bIsRunning);
	DOREPLIFETIME(ABatteryMan, IsAnimationBlended);
	DOREPLIFETIME(ABatteryMan, bIsKeyboardEnabled);
	DOREPLIFETIME(ABatteryMan, bIsRoll);
	DOREPLIFETIME(ABatteryMan, Power);
	DOREPLIFETIME(ABatteryMan, CurrentComboCount);
	DOREPLIFETIME(ABatteryMan, RollIndex);
	DOREPLIFETIME(ABatteryMan, MoveForwardValue);
	DOREPLIFETIME(ABatteryMan, MoveRightValue);
	DOREPLIFETIME(ABatteryMan, AllComboCount);
	DOREPLIFETIME(ABatteryMan, bInputMoveFwd);
	DOREPLIFETIME(ABatteryMan, bInputMoveBack);
	DOREPLIFETIME(ABatteryMan, bInputMoveLeft);
	DOREPLIFETIME(ABatteryMan, bInputMoveRight);
	DOREPLIFETIME(ABatteryMan, bIsMoving);
	DOREPLIFETIME(ABatteryMan, InputMoveDirection);
	DOREPLIFETIME(ABatteryMan, DirectionMotion);
	DOREPLIFETIME(ABatteryMan, MovingSpeed);
	DOREPLIFETIME(ABatteryMan, MoveFwdBwdSum);
	DOREPLIFETIME(ABatteryMan, MoveRgtLftSum);
	DOREPLIFETIME(ABatteryMan, MoveRightDegreesMutiplier);
	DOREPLIFETIME(ABatteryMan, ControlRotation);
	DOREPLIFETIME(ABatteryMan, bIsCrouching);
	DOREPLIFETIME(ABatteryMan, bIsChangeableRotation);
}

void ABatteryMan::OnRep_IsRoll()
{
		RollInput();
}

void ABatteryMan::OnRep_IsArmed()
{
	if (IsLocallyControlled()) {
		if (bIsArmed) {
			GetCharacterMovement()->MaxWalkSpeed = MaxArmedSpeed;
		}
		else {
			if (bIsRunning) {
				GetCharacterMovement()->MaxWalkSpeed = MaxRunSpeed;
			}
			else {
				GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
			}
		}
	}
}

void ABatteryMan::OnRep_IsRunning()
{
	if (IsLocallyControlled()) {
		if (bIsRunning) {
			GetCharacterMovement()->MaxWalkSpeed = MaxRunSpeed;
		}
		else {
			GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
		}
	}
}

void ABatteryMan::OnRep_ChangeCurrentCombo()
{
	if (IsLocallyControlled()) {
		AProjectPlayerState* PS = GetPlayerState<AProjectPlayerState>();
		if (PS) {
			PS->ComboCount = CurrentComboCount;
			if (ProjectPC) {
				ProjectPC->SetComboCount(CurrentComboCount, AllComboCount);
			}
		}
	}
}

void ABatteryMan::FireLineTrace()
{
	log(ELogLevel::WARNING, __FUNCTION__, ELogOutput::ALL);
	FVector start;
	FVector end;

	const float Spread = FMath::DegreesToRadians(LineTraceSpread * 0.5f);

	if (LineTraceType == ELineTraceType::CAMERA_SINGLE||LineTraceType==ELineTraceType::CAMERA_SPREAD) {
		FVector CameraLoc = FollowCamera->GetComponentLocation();
		FRotator CameraRot = FollowCamera->GetComponentRotation();
		start = CameraLoc;

		if (LineTraceType == ELineTraceType::CAMERA_SPREAD) {
			end = CameraLoc + FMath::VRandCone(CameraRot.Vector(), Spread, Spread)*LineTraceDistance;
		}else{
			end = CameraLoc + CameraRot.Vector()*LineTraceDistance;
		}

	}
	else if (LineTraceType == ELineTraceType::PLAYER_SINGLE||LineTraceType==ELineTraceType::PLAYER_SPREAD) {
		FVector PlayerEyesLoc;
		FRotator PlayerEyesRot;

		GetActorEyesViewPoint(PlayerEyesLoc, PlayerEyesRot);

		start = PlayerEyesLoc;

		if (LineTraceType == ELineTraceType::PLAYER_SPREAD) {
			end = PlayerEyesLoc + FMath::VRandCone(PlayerEyesRot.Vector(), Spread, Spread)*LineTraceDistance;
		}
		else {
			end = PlayerEyesLoc + PlayerEyesRot.Vector()*LineTraceDistance;
		}
	}

	FHitResult HitDetails = FHitResult(ForceInit);
	FCollisionQueryParams TraceParams(FName(TEXT("LineTraceParameters")), true, NULL);
	TraceParams.bTraceComplex = true;
	TraceParams.bReturnPhysicalMaterial = true;

	bool bIsHit = GetWorld()->LineTraceSingleByChannel(HitDetails, start, end, ECC_GameTraceChannel1, TraceParams);
	if (bIsHit) {
		log(ELogLevel::INFO, "We hit something", ELogOutput::ALL);
		DrawDebugLine(GetWorld(), start, end, FColor::Blue, false, 5.f, ECC_WorldStatic, 1.f);
		log(ELogLevel::WARNING, HitDetails.Actor->GetName(), ELogOutput::ALL);
		log(ELogLevel::DEBUG, FString::SanitizeFloat(HitDetails.Distance), ELogOutput::ALL);
		DrawDebugBox(GetWorld(), HitDetails.ImpactPoint, FVector(2.f, 2.f, 2.f), FColor::Purple, false, 5.f, ECC_WorldStatic, 1.f);
	}
	else {
		log(ELogLevel::WARNING, "We hit nothing", ELogOutput::ALL);
		DrawDebugLine(GetWorld(), start, end, FColor::Yellow, false, 5.f, ECC_WorldStatic, 1.f);
	}
}

void ABatteryMan::log(ELogLevel logLevel, FString Message, ELogOutput LogOutput)
{
	if ((LogOutput == ELogOutput::ALL || LogOutput == ELogOutput::SCREEN)&&GEngine) {
		FColor logColor = FColor::Cyan;
		switch (logLevel)
		{
		case ELogLevel::TRACING:
			logColor = FColor::Green;
			break;
		case ELogLevel::DEBUG:
			logColor = FColor::Cyan;
			break;
		case ELogLevel::INFO:
			logColor = FColor::White;
			break;
		case ELogLevel::WARNING:
			logColor = FColor::Yellow;
			break;
		case ELogLevel::ERROR:
			logColor = FColor::Red;
			break;
		default:
			break;
		}
		GEngine->AddOnScreenDebugMessage(-1, 4.5F, logColor, Message);
	}
}

void ABatteryMan::RestartGame()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);

}

void ABatteryMan::PunchAttack()
{
	//AttackInput(EAttackType::MELEE_FIST);
	ServerFist();
}

void ABatteryMan::KickAttack()
{
	//AttackInput(EAttackType::MELEE_KICK);
	ServerKick();
}

void ABatteryMan::AttackInput(EAttackType AttackType)
{
	if (PlayerAttackDataTable) {
		static const FString ContextString(TEXT("PlayerAttackMontageContext"));

		const FAttachmentTransformRules attachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);

		CurrentAttack = AttackType;

		AttackStrength = EAttackStrength::MEDIUM;
		if (bIsAttackStrong) {
			AttackStrength = EAttackStrength::STRONG;
		}
		else if (bIsAttackLight && !bIsAttackStrong) {
			AttackStrength = EAttackStrength::LIGHT;
		}
		FName RowKey;
		switch (AttackType) {
		case EAttackType::MELEE_FIST:
			RowKey = FName(TEXT("Punch2"));

			left_MeleeCollision->AttachToComponent(GetMesh(), attachRules, "left_fist");
			right_MeleeCollision->AttachToComponent(GetMesh(), attachRules, "right_fist");

			//bIsKeyboardEnabled = true;

			//IsAnimationBlended = true;
			break;
		case EAttackType::MELEE_KICK:
			RowKey = FName(TEXT("Kick"));

			left_MeleeCollision->AttachToComponent(GetMesh(), attachRules, "foot_l_collision");
			right_MeleeCollision->AttachToComponent(GetMesh(), attachRules, "foot_r_collision");

			//bIsKeyboardEnabled = false;

			//IsAnimationBlended = false;
			break;
		default:
			IsAnimationBlended = true;
			break;
		}
		AttackMontageEdit = PlayerAttackDataTable->FindRow<FPlayerAttackMontageEdit>(RowKey, ContextString, true);
		if (AttackMontageEdit) {
			int MontageSectionIndex/* = rand() % (AttackMontageEdit->countOfSections) + 1*/;
			switch (AttackStrength) {
			case EAttackStrength::LIGHT:
				MontageSectionIndex = 1;
				break;
			case EAttackStrength::MEDIUM:
				MontageSectionIndex = 2;
				break;
			case EAttackStrength::STRONG:
				MontageSectionIndex = 3;
				break;
			default:
				MontageSectionIndex = 2;
				break;
			}
			FString MontageSection = "Start" + FString::FromInt(MontageSectionIndex);
			PlayAnimMontage(AttackMontageEdit->AnimMontage, 1.f, FName(*MontageSection));
		}
	}
}

void ABatteryMan::RollInput()
{
	if (RollIndex != 3||(RollIndex==3&&GetVelocity().Z<-50.f)) {
	bIsKeyboardEnabled = false;
	IsAnimationBlended = false;
	FString MontageSection = "Start" + FString::FromInt(RollIndex);
	UAnimInstance * AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance) {
		UPlayerAnimInstance* PlayerAnimInstance = Cast<UPlayerAnimInstance>(AnimInstance);
		if (PlayerAnimInstance) {
			PlayerAnimInstance->SetRootMotionMode(ERootMotionMode::RootMotionFromEverything);
		}
	}
		PlayAnimMontage(RollMontage, 1.f, FName(MontageSection));
	}
}
