// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthBarWidget.h"
#include "Components/ProgressBar.h"
#include "Components/WidgetComponent.h"


void UHealthBarWidget::UpdateHealth(float CurrentHealth, float MaxHealth)
{
    // HPBar가 연결되어 있지 않으면 종료
    if (!IsValid(HPBar))
    {
        return;
    }

    // 최대 체력이 0 이하이면 빈 막대로 표시
    if (MaxHealth <= 0.0f)
    {
        HPBar->SetPercent(0.0f);
        return;
    }

    // 현재 체력을 0~1 비율로 계산
    const float HealthPercent = FMath::Clamp(CurrentHealth / MaxHealth, 0.0f, 1.0f);

    // 계산한 비율을 HP바에 반영
    HPBar->SetPercent(HealthPercent);
}

void UHealthBarWidget::SetupHealthBarComponent(UWidgetComponent* InComponent, USceneComponent* Inparent, float Height)
{
	if (!InComponent || !Inparent)
	{
		return;
	}

	InComponent->SetupAttachment(Inparent);
    
    InComponent->SetRelativeLocation(FVector(0.0f, 0.0f, Height));

    InComponent->SetWidgetSpace(EWidgetSpace::Screen);
	InComponent->SetDrawSize(FVector2D(200.0f, 20.0f));
    InComponent->SetPivot(FVector2D(0.5f, 0.5f));

    InComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}
