// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBarWidget.generated.h"


class UProgressBar;
class UWidgetComponent;
class USceneComponent;
class UTextBlock;

UCLASS()
class PROJECTSTEALTH_API UHealthBarWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void UpdateHealth(float CurrentHealth, float MaxHealth);
    static void SetupHealthBarComponent(UWidgetComponent* InComponent, USceneComponent* Inparent, float Height = 120.0f);
    
public:
	void SetAssassinationPromptVisible(bool bVisible);

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> AssassinationPrompt;

private:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> HPBar;
};