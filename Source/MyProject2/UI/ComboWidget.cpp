// Fill out your copyright notice in the Description page of Project Settings.


#include "ComboWidget.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"

UComboWidget::UComboWidget(const FObjectInitializer& ObjectInitializer):UUserWidget(ObjectInitializer) {
	
}

void UComboWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!MessageButton->OnClicked.IsBound()) {
		MessageButton->OnClicked.AddDynamic(this, &UComboWidget::OnClick_MessageButton);
	}

	StoreWidgetAnimations();

	ComboFadeAnimation = GetAnimationByName(TEXT("ComboFade"));
	ComboShakeAnimation = GetAnimationByName(TEXT("ComboShake"));
}

void UComboWidget::UpdateComboCount(int32 value)
{
	if (TXTCombo&&value > 1) {
		if (TXTCombo->Visibility == ESlateVisibility::Hidden) {
			TXTCombo->SetVisibility(ESlateVisibility::Visible);
		}
		TXTCombo->SetText(FText::FromString(FString::FromInt(value) + "x combo"));

		if (ComboFadeAnimation) {
			PlayAnimation(ComboFadeAnimation, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
		}
		if (ComboShakeAnimation) {
			PlayAnimation(ComboShakeAnimation, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
		}
	}
}

void UComboWidget::ResetCombo()
{
	if (TXTCombo) {
		TXTCombo->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UComboWidget::StoreWidgetAnimations()
{
	AnimationsMap.Empty();

	FProperty* Prop = GetClass()->PropertyLink;
	while (Prop) {
		if (Prop->GetClass() == FObjectProperty::StaticClass()) {
			FObjectProperty* ObjProp = Cast<FObjectProperty>(Prop);
			if (ObjProp->PropertyClass == UWidgetAnimation::StaticClass()) {
				UObject* Obj = ObjProp->GetObjectPropertyValue_InContainer(this);
				UWidgetAnimation* WidgetAnimation = Cast<UWidgetAnimation>(Obj);
				if (WidgetAnimation&&WidgetAnimation->MovieScene) {
					FName AniName = WidgetAnimation->MovieScene->GetFName();
					GEngine->AddOnScreenDebugMessage(-1, 4.5f, FColor::Magenta, AniName.ToString());
					AnimationsMap.Add(AniName, WidgetAnimation);
				}
			}
		}
		Prop = Prop->PropertyLinkNext;
	}
}

UWidgetAnimation * UComboWidget::GetAnimationByName(FName AnimationName) const
{
	UWidgetAnimation* const* WidgetAnimation = AnimationsMap.Find(AnimationName);
	if (WidgetAnimation) {
		return *WidgetAnimation;
	}
	return nullptr;
}

void UComboWidget::ChangeComboCount_Implementation(int ComboCount, int AllCombo)
{
}

void UComboWidget::OnClick_MessageButton_Implementation()
{
	/*if (Message_Box_Class != nullptr&&MessageBox == nullptr) {
		MessageBox = CreateWidget<UMessageWidget>(GetWorld(), Message_Box_Class);
		MessageBox->AddToViewport();
	}*/
}
