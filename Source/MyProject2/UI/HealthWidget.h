// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/UMG.h"
#include "HealthWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT2_API UHealthWidget : public UUserWidget
{
	GENERATED_BODY()
	
	UHealthWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;
};
