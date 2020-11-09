// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "UI/InGameHUD.h"
#include "RotationActor.h"
#include "Sound/SoundCue.h"
#include "Blueprint/UserWidget.h"
#include "Engine/DataTable.h"
#include "BatteryMan_GameMode.h"
#include "PlayerAnimInstance.h"
#include "BatteryMan.generated.h"


class UBoxComponent;

UENUM(BlueprintType)
enum class ELineTraceType :uint8 {
	CAMERA_SINGLE	UMETA(DisplayValue = "Camera - SingleTrace"),
	PLAYER_SINGLE	UMETA(DisplayValue = "Player - SingleTrace"),
	CAMERA_SPREAD	UMETA(DisplayValue = "Camera - Spread"),
	PLAYER_SPREAD	UMETA(DisplayValue = "Player - Spread")
};

UENUM(BlueprintType)
enum class ELogLevel :uint8 {
	TRACING			UMETA(DisplayName="Trace"),
	DEBUG			UMETA(DisplayName="Debug"),
	INFO			UMETA(DisplayName="Info"),
	WARNING			UMETA(DisplayName="Warning"),
	ERROR			UMETA(DisplayName="Error")
};

UENUM(BlueprintType)
enum class ELogOutput :uint8 {
	ALL				UMETA(DisplayName="All"),
	OUTPUT_LOG		UMETA(DisplayName="Output log"),
	SCREEN			UMETA(DisplayName="Screen")
};

UENUM(BlueprintType)
enum class EAttackType :uint8 {
	MELEE_FIST		UMETA(DisplayName="Melee - Fist"),
	MELEE_KICK		UMETA(DisplayName="Melee - Kick")
};

UENUM(BlueprintType)
enum class EAttackStrength :uint8 {
	LIGHT			UMETA(DisplayName="Light"),
	MEDIUM			UMETA(DisplayName="Medium"),
	STRONG			UMETA(DisplayName="Strong")
};

UENUM(BlueprintType)
enum class EActiveState:uint8 {
	No,
	Yes = 80
};

USTRUCT(BlueprintType)
struct FPickedItem
{
	GENERATED_BODY()

	class ARotationActor* RotActor = nullptr;
	class ABattery* Battery = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 indexOfPickedItem=0;

	FPickedItem() {
		RotActor = nullptr;
		Battery = nullptr;
		indexOfPickedItem = 10;
	}
};

USTRUCT(BlueprintType)
struct FPlayerAttackMontage : public FTableRowBase 
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UAnimMontage* AnimMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int32 indexOfSection = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString Description;

};

USTRUCT(BlueprintType)
struct FPlayerAttackMontageEdit : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UAnimMontage* AnimMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int32 countOfSections = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString Description;

};

UCLASS()
class MYPROJECT2_API ABatteryMan : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABatteryMan();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		class UCameraComponent* FollowCamera;

	AInGameHUD* InGameHUD;

	void MoveForward(float Axis);
	void MoveRight(float Axis);

	bool bDead;

	ABatteryMan_GameMode* GameMode;

	UPROPERTY(EditAnywhere)
		float timeDilation;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Power|Power")
		float Power;

	UPROPERTY(EditAnywhere, Category = "Power|PowerThresold")
		float Power_Treshold;

	UPROPERTY(EditAnywhere)
		FVector VectorUp2;

	UFUNCTION()
		void OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor,
			class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
		void EndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable, Category = "Animation")
		bool GetIsAnimationBlended();

	UFUNCTION(BlueprintCallable, Category = "Animation")
		void StopRollMontage();

	UFUNCTION(BlueprintCallable, Category = "Animation")
		bool IsArmed();

	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool IsRunning();

	UFUNCTION(BlueprintCallable, Category = "Animation")
	float GetMoveRight();

	UFUNCTION(BlueprintCallable, Category = "Animation")
	float GetMoveForward();

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void SetIsKeyboardEnabled(bool Enabled);

	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool GetAnimRoll();

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void SetAnimRoll(bool isRoll);

	UFUNCTION(BlueprintCallable, Category = "Animation")
	EAttackType GetCurrentAttack();

	UFUNCTION(BlueprintPure, Category = "Health")
	FORCEINLINE float GetMaxHealth() const{return MaxHealth;}

	UFUNCTION(BlueprintPure, Category = "Health")
	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintCallable, Category="Health")
	void SetCurrentHealth(float healthValue);

	UFUNCTION(BlueprintCallable, Category = "Health")
	float TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(Server, Reliable)
	void ServerKick();
	bool ServerKick_Validation();
	void ServerKick_Implementation();

	UFUNCTION(NetMulticast, Reliable)
	void ClientKick();
	bool ClientKick_Validation();
	void ClientKick_Implementation();

	UFUNCTION(Server, Reliable)
	void ServerFist();
	void ServerFist_Implementation();

	UFUNCTION(NetMulticast, Reliable)
	void MultiFist();
	void MultiFist_Implementation();

	UFUNCTION(NetMulticast, Reliable)
	void StopMontage();
	void StopMontage_Implementation();

	UFUNCTION(Server, Reliable)
	void SetIsRunningServer(bool bIsRun);
	void SetIsRunningServer_Implementation(bool bIsRun);

	UFUNCTION(Server, Reliable)
	void SetArmedServer(bool bIsArm);
	void SetArmedServer_Implementation(bool bIsArm);

	UFUNCTION(NetMulticast, Reliable)
	void SetArmedMulti(bool bIsArm);
	void SetArmedMulti_Implementation(bool bIsArm);

	UFUNCTION(Server, Reliable)
	void SetStrongAttackServer(bool bIsStrong);
	void SetStrongAttackServer_Implementation(bool bIsStrong);

	UFUNCTION(Server, Reliable)
	void SetLightAttackServer(bool bIsLight);
	void SetLightAttackServer_Implementation(bool bIsLight);

	UFUNCTION(Server, Reliable)
	void RollServer();
	void RollServer_Implementation();

	UFUNCTION(Server, Reliable)
	void ComboServer(int32 ComboCount);
	void ComboServer_Implementation(int32 ComboCount);

	UFUNCTION(Server, Reliable)
	void MoveForwardServer(float Axis);
	void MoveForwardServer_Implementation(float Axis);

	UFUNCTION(Server, Reliable)
	void MoveRightServer(float Axis);
	void MoveRightServer_Implementation(float Axis);

	UFUNCTION(Server, Reliable)
	void CrouchStartServer();
	void CrouchStartServer_Implementation();

	UFUNCTION(Server, Reliable)
	void CrouchEndServer();
	void CrouchEndServer_Implementation();

	UFUNCTION(Server, Reliable)
	void SetCurrentControlRotation(FRotator ControlRotator);
	void SetCurrentControlRotation_Implementation(FRotator ControlRotator);

	UFUNCTION(Server, Reliable)
	void ChangeOrientToMove();
	void ChangeOrientToMove_Implementation();

	UFUNCTION(Server, Reliable)
	void ChangeActorRotation(FRotator ControllerRotation);
	void ChangeActorRotation_Implementation(FRotator ControllerRotation);

	UPROPERTY(EditAnywhere, Category = "UI HUD")
	TSubclassOf<UUserWidget>Player_Power_Widget_Class;
	UUserWidget* Player_Power_Widget;
	UUserWidget* Player_Menu_Widget;

	UPROPERTY(Replicated)
	bool bInputMoveFwd;
	UPROPERTY(Replicated)
	bool bInputMoveBack;
	UPROPERTY(Replicated)
	bool bInputMoveLeft;
	UPROPERTY(Replicated)
	bool bInputMoveRight;
	UPROPERTY(Replicated)
	bool bIsMoving;
	UPROPERTY(Replicated)
	float InputMoveDirection;
	UPROPERTY(Replicated)
	float DirectionMotion;
	UPROPERTY(Replicated)
	float MovingSpeed;
	UPROPERTY(Replicated)
	int MoveFwdBwdSum;
	UPROPERTY(Replicated)
	int MoveRgtLftSum;
	UPROPERTY(Replicated)
	float MoveRightDegreesMutiplier;
	UPROPERTY(Replicated)
	FRotator ControlRotation;
	UPROPERTY(Replicated)
	bool bIsCrouching;	
	UPROPERTY(Replicated)
	bool bIsChangeableRotation;
	void UpdateInputState();
	float GetRelativeRotation();

	AActor* aactor;
	FLatentActionInfo latentInfo;
	TSubclassOf<AActor> needClass;
	TArray<AActor*> allActors;
	FVector myPosition;
	TTuple<float, int32> rotationActorParams(ARotationActor* rotationActor);
	FTimerHandle Timer1;
	EActiveState activeState;
	FPickedItem pickedItem;
	class AProjectPlayerController* ProjectPC;
	void ShowHideWidget();
	UPROPERTY(Replicated)
	bool IsAnimationBlended;

	class UAnimMontage* animMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AttackData")
		UDataTable* attackData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AttackData")
		UDataTable* PlayerAttackDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* MeleeFistAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Collision", meta = (AllowPrivateAccess = "true"))
		UBoxComponent* left_MeleeCollision;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Collision", meta = (AllowPrivateAccess = "true"))
		UBoxComponent* right_MeleeCollision;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio", meta=(AllowPrivateAccess = "true"))
		USoundCue* punchSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
		USoundCue* punchThrowSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
		UAudioComponent* punchAudioComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
		UAudioComponent* punchAudioThrowComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Possession")
	class UMaterialInstance* DefaultMaterialBody;

	UPROPERTY(EditDefaultsOnly, Category = "Possession")
		class UMaterialInstance* DefaultMaterialChest;

	UPROPERTY(EditDefaultsOnly, Category = "Possession")
		class UMaterialInstance* PossessedMaterialBody;

	UPROPERTY(EditDefaultsOnly, Category = "Possession")
		class UMaterialInstance* PossessedMaterialChest;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
		float AnimationVarible;

	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float MaxHealth;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth)
	float CurrentHealth;

	bool bRollFalling;

	UFUNCTION()
	void OnRep_CurrentHealth();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FPlayerAttackMontage* AttackMontage;

	FPlayerAttackMontageEdit* AttackMontageEdit;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UAnimMontage* RollMontage;

	UPROPERTY(Replicated)
	bool bIsKeyboardEnabled;
	UPROPERTY(ReplicatedUsing = OnRep_IsRunning)
	bool bIsRunning;
	UPROPERTY(ReplicatedUsing = OnRep_IsArmed)
	bool bIsArmed;
	UPROPERTY(ReplicatedUsing = OnRep_IsRoll)
	bool bIsRoll;
	UPROPERTY(Replicated)
	int RollIndex;
	UPROPERTY(Replicated)
	int AllComboCount;

	UFUNCTION()
	void OnRep_IsRoll();

	UFUNCTION()
	void OnRep_IsArmed();

	UFUNCTION()
	void OnRep_IsRunning();

	UPROPERTY(Replicated)
	bool bIsAttackLight;
	UPROPERTY(Replicated)
	bool bIsAttackStrong;

	FTimerHandle ArmedToIdleTimerHandle;
	int32 CountdownToIdle;
	UPROPERTY(Replicated)
	float MoveForwardValue;
	UPROPERTY(Replicated)
	float MoveRightValue;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	int32 MaxCountdownToIdle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	float MaxCrouchSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	float MaxRunSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	float MaxWalkSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	float MaxArmedSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	EAttackStrength AttackStrength;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Line Trace")
	ELineTraceType LineTraceType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Line Trace")
	float LineTraceDistance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Line Trace")
	float LineTraceSpread;

	UFUNCTION()
	void OnRep_ChangeCurrentCombo();

	void FireLineTrace();

	void log(ELogLevel logLevel, FString Message, ELogOutput LogOutput);

	void RestartGame();
	void PunchAttack();
	void KickAttack();
	void AttackInput(EAttackType AttackType);
	void RollInput();
	void AttackStart();
	void AttackEnd();
	void FinishAttack();

	void LightAttackStart();
	void LightAttackEnd();
	void StrongAttackStart();
	void StrongAttackEnd();

	FTimerHandle ComboResetHandle;

	bool bIsCurrentlyPossessed;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type Reason) override;
	void EndPlay1();
	void TimeSlow();
	void TimeNormal();
	void AttackLeft();
	void AttackRight();
	UFUNCTION(BlueprintCallable)
	void OnAttackHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void ResetCombo();

	UFUNCTION()
		void CrouchStart();

	UFUNCTION()
		void CrouchEnd();

	UFUNCTION()
		void ArmPlayer(bool Value);

	UFUNCTION()
	void ArmPlayerImmediately();

	UFUNCTION()
	void RunStart();

	UFUNCTION()
	void RunEnd();

	UFUNCTION()
	void TriggerCountdownToIdle();

	UFUNCTION()
	void Interact();

	void OnHealthUpdate();

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay|Projectile")
		TSubclassOf<class ABatteryManMPProjectile> ProjectileClass;

	/** Delay between shots in seconds. Used to control fire rate for our test projectile, but also to prevent an overflow of server functions from binding SpawnProjectile directly to input.*/
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
		float FireRate;

	/** If true, we are in the process of firing projectiles. */
	bool bIsFiringWeapon;

	/** Function for beginning weapon fire.*/
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void StartFire();

	/** Function for ending weapon fire. Once this is called, the player can use StartFire again.*/
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void StopFire();

	/** Server function for spawning projectiles.*/
	UFUNCTION(Server, Reliable)
		void HandleFire();

	/** A timer handle used for providing the fire rate delay in-between spawns.*/
	FTimerHandle FiringTimer;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	AController* SavedController;
	UPROPERTY(ReplicatedUsing = OnRep_ChangeCurrentCombo)
	int32 CurrentComboCount;
	EAttackType CurrentAttack;

};
