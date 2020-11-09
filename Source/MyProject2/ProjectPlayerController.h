// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "ProjectGameState.h"
#include "ProjectPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT2_API AProjectPlayerController : public APlayerController
{
	GENERATED_BODY()

		AProjectPlayerController();

public:

	UPROPERTY(EditDefaultsOnly, Category = "UMG")
	TSubclassOf<UUserWidget> Main_Menu_Widget_Class;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UMG")
	class UComboWidget* Main_Menu_Widget;

	UPROPERTY(EditAnywhere, Category = "UI HUD")
	TSubclassOf<UUserWidget> Player_Health_Widget_Class;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI HUD")
	class UHealthWidget* Player_Health_Widget;

	UPROPERTY(EditAnywhere, Category = "UI HUD")
		TSubclassOf<UUserWidget> Instruction_Widget_Class;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI HUD")
		class UMainMenuWidget* Instruction_Widget;

	UFUNCTION(Server, Reliable)
	void CreateMenuWidgetServer();
	void CreateMenuWidgetServer_Implementation();

	UFUNCTION(Client, Reliable)
	void CreateMenuWidget();
	void CreateMenuWidget_Implementation();

	UFUNCTION(BlueprintImplementableEvent, Category = "UMG")
	void SetComboCount(int Count, int32 allCount);
	void SetComboCount_Implementation(int Count, int32 allCount);

	UFUNCTION(BlueprintCallable)
	void AddMessages(FMessageInfo MessageInfo);

	UFUNCTION(Server, Reliable)
	void AddMessagesServer(FMessageInfo MessageInfo);
	void AddMessagesServer_Implementation(FMessageInfo MessageInfo);

	UFUNCTION(BlueprintImplementableEvent)
	void GetMessages(FMessageInfo Message);

protected:
	virtual void BeginPlay() override;
};
