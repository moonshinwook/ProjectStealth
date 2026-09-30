// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBarWidget.generated.h"


class UProgressBar;
class UWidgetComponent;
class USceneComponent;



UCLASS()
class PROJECTSTEALTH_API UHealthBarWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void UpdateHealth(float CurrentHealth, float MaxHealth);
    static void SetupHealthBarComponent(UWidgetComponent* InComponent, USceneComponent* Inparent, float Height = 120.0f);


private:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> HPBar;
};