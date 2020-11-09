// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "BatteryMan.h"
#include "ProjectPlayerState.h"
#include "BatteryMan.h"
#include "UI/ComboWidget.h"
#include "UI/HealthWidget.h"
#include "UI/MainMenuWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "ProjectGameState.h"
#include "Engine/Engine.h"

AProjectPlayerController::AProjectPlayerController(){

}

void AProjectPlayerController::SetComboCount_Implementation(int Count, int32 allCount)
{
}

void AProjectPlayerController::AddMessages(FMessageInfo MessageInfo)
{
	AddMessagesServer(MessageInfo);
}

void AProjectPlayerController::AddMessagesServer_Implementation(FMessageInfo MessageInfo)
{
	if (HasAuthority()) {
		AProjectGameState* GS = Cast<AProjectGameState>(UGameplayStatics::GetGameState(GetWorld()));
		if (GS) {
			GS->Messages.Add(MessageInfo);
		}
	}
}

void AProjectPlayerController::BeginPlay() {

	Super::BeginPlay();

	UWidgetBlueprintLibrary::SetInputMode_GameOnly(this);

	bShowMouseCursor = true;

	if(!HasAuthority())
	CreateMenuWidgetServer();
}

void AProjectPlayerController::CreateMenuWidgetServer_Implementation()
{
	if (HasAuthority()) {
		CreateMenuWidget();
	}
}

void AProjectPlayerController::CreateMenuWidget_Implementation() {
	if (Main_Menu_Widget_Class != nullptr) {
		Main_Menu_Widget = CreateWidget<UComboWidget>(GetWorld(), Main_Menu_Widget_Class);
		if (Main_Menu_Widget) {
			Main_Menu_Widget->AddToViewport();
		}
		//AProjectPlayerState* PS = GetPlayerState<AProjectPlayerState>();
		ABatteryMan* BM = Cast<ABatteryMan>(GetPawnOrSpectator());
		if (BM != nullptr) {
			BM->ProjectPC = this;
		}
	}
	if (Player_Health_Widget_Class != nullptr) {
		Player_Health_Widget = CreateWidget<UHealthWidget>(GetWorld(), Player_Health_Widget_Class);
		if (Player_Health_Widget) {
			Player_Health_Widget->AddToViewport();
		}
	}
	if (Instruction_Widget_Class != nullptr) {
		Instruction_Widget = CreateWidget<UMainMenuWidget>(GetWorld(), Instruction_Widget_Class);
		if (Instruction_Widget) {
			Instruction_Widget->AddToViewport();
		}
	}
}



