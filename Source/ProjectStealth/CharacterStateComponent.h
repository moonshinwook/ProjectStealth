// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterStateComponent.generated.h"

UENUM(BlueprintType)
enum class ELifeState : uint8
{
	Alive,
	Dead,
};

UENUM(BlueprintType)
enum class EDetectionState : uint8
{
	Detected,
	Hidden,
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTSTEALTH_API UCharacterStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCharacterStateComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(visibleAnywhere, BlueprintReadOnly, Category = "State", meta = (AllowprivateAccess="true"))
	ELifeState LifeState = ELifeState::Alive;
	UPROPERTY(visibleAnywhere, BlueprintReadOnly, Category = "State", meta = (AllowprivateAccess = "true"))
	EDetectionState PlayerDetectionState = EDetectionState::Hidden;

public:
	// 생존 상태인지 확인
	bool IsAlive() const
	{
		return LifeState == ELifeState::Alive;
	}
	//	사망 상태로 변경
	void MarkDead()
	{
		LifeState = ELifeState::Dead;
	}

public:
	// 발각 상태인지 확인
	bool IsDetected() const
	{
		return PlayerDetectionState == EDetectionState::Detected;
	}

	// 발각 상태로 변경
	void MarkDetected()
	{
		PlayerDetectionState = EDetectionState::Detected;
	}

	// 미발각 상태로 변경
	void MarkHidden()
	{
		PlayerDetectionState = EDetectionState::Hidden;
	}
};
