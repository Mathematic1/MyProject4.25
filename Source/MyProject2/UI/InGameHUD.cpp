// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameHUD.h"
#include "ComboWidget.h"

AInGameHUD::AInGameHUD() {

}

void AInGameHUD::DrawHUD()
{
	Super::DrawHUD();
}

void AInGameHUD::BeginPlay()
{
	Super::BeginPlay();

	if (ComboWidgetClass) {
		ComboWidget = CreateWidget<UComboWidget>(GetWorld(), ComboWidgetClass);
		if (ComboWidget) {
			ComboWidget->AddToViewport();
		}
	}
}

void AInGameHUD::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AInGameHUD::UpdateComboCount(int32 value)
{
	if (ComboWidget) {
		ComboWidget->UpdateComboCount(value);
	}
}

void AInGameHUD::ResetCombo()
{
	if (ComboWidget) {
		ComboWidget->ResetCombo();
	}
}


