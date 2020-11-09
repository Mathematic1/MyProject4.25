// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthWidget.h"
#include "Blueprint/UserWidget.h"

UHealthWidget::UHealthWidget(const FObjectInitializer& ObjectInitializer) :UUserWidget(ObjectInitializer) {

}

void UHealthWidget::NativeConstruct() {
	Super::NativeConstruct();
}
