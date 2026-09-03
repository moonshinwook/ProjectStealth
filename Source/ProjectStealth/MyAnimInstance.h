// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSTEALTH_API UMyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

private:
	UPROPERTY(Category = "Character Move", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool ShouldMove;
	UPROPERTY(Category = "Character Move", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float Horizontal;
	UPROPERTY(Category = "Character Move", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float Vertical;


	UPROPERTY(VisibleAnywhere)
	class ACharacter* Character;
	UPROPERTY(VisibleAnywhere)
	class UCharacterMovementComponent* CharacterMovement;
	UPROPERTY(VisibleAnywhere)
	UAnimMontage* RollMontage;
	UPROPERTY(VisibleAnywhere)
	UAnimMontage* AttackMontage;

public:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
public:
	UMyAnimInstance(); // 생성자 추가
public:
	void PlayRollMontage();
	void PlayAttackMontage();
};
