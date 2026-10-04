// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStateComponent.h"

// Sets default values for this component's properties
UCharacterStateComponent::UCharacterStateComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCharacterStateComponent::BeginPlay()
{
	Super::BeginPlay();

	//	잘못된 체력 설정 방지
	MaxHealth = FMath::Max(MaxHealth, 1.0f);

	//	설정된 최대 체력으로 시작
	CurrentHealth = MaxHealth;
}


// Called every frame
void UCharacterStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

float UCharacterStateComponent::ReceiveDamage(float Damage)
{
	if (!IsAlive() || Damage <= 0.0f)
	{
		return 0.0f;
	}
	
	const float PreviousHealth = CurrentHealth;

	//	체력이 음수가 되지 않도록 제한
	CurrentHealth = FMath::Max(CurrentHealth - Damage, 0.0f);

	//	체력이 소진되면 사망 상태로 변경
	if (CurrentHealth <= 0.0f)
	{
		MarkDead();
	}

	UE_LOG(LogTemp, Log, TEXT("[%s] Health: %.1f / %.1f | Alive: %s"), *GetNameSafe(GetOwner()), CurrentHealth,MaxHealth, IsAlive() ? TEXT("true") : TEXT("false"));
	
	//	실제 감소한 체력 반환
	return PreviousHealth - CurrentHealth;
}

