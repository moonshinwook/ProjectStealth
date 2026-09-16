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
	UPROPERTY(Category = "Character Move", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool bIsCrouched;




	UPROPERTY(VisibleAnywhere)
	class AMyCharacter* Character;
	UPROPERTY(VisibleAnywhere)
	class UCharacterMovementComponent* CharacterMovement;
	UPROPERTY(VisibleAnywhere)

	// Animation montages
	UAnimMontage* RollMontage;
	UPROPERTY(VisibleAnywhere)
	UAnimMontage* AttackMontage;
	UPROPERTY(VisibleAnywhere)
	UAnimMontage* AssassinationAttackMontage;
	

public:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
public:
	UMyAnimInstance(); // 생성자 추가
public:
	void PlayRollMontage();
	void PlayAttackMontage();
	void PlayAssassinationAttackMontage();
};
