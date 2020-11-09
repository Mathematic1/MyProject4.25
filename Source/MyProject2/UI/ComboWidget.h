// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/UMG.h"
#include "MessageWidget.h"
#include "ComboWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT2_API UComboWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UComboWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	void UpdateComboCount(int32 value);

	void ResetCombo();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* TXTCombo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* MessageButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		TSubclassOf<UMessageWidget> Message_Box_Class;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UMessageWidget* MessageBox;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void ChangeComboCount(int ComboCount, int AllCombo);
	void ChangeComboCount_Implementation(int ComboCount, int AllCombo);

	float HeightList;

	UFUNCTION(BlueprintImplementableEvent)
	void OnClick_MessageButton();
	void OnClick_MessageButton_Implementation();

	void StoreWidgetAnimations();

	UWidgetAnimation* GetAnimationByName(FName AnimationName) const;
	
private:
	TMap<FName, UWidgetAnimation*> AnimationsMap;

	UWidgetAnimation* ComboFadeAnimation;
	UWidgetAnimation* ComboShakeAnimation;
};
