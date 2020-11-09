// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/UMG.h"
#include "MessageWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT2_API UMessageWidget : public UUserWidget
{
	GENERATED_BODY()
	
	UMessageWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

};
