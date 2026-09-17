// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "MyCharacter.generated.h"

class UAnimMontage;

UCLASS()
class PROJECTSTEALTH_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere)
	class UMyAnimInstance* AnimInstance;

public:
	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* Camera;

public:
	// Sets default values for this character's properties
	AMyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Assassination", Meta = (ClampMin = 0.0f))
	float AssassinationRange = 150.0f;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
public:
	void KeyUpDown(float value);
	void KeyLeftRight(float value);
public:
	void KeyLookUpDown(float value);
	void KeyLookLeftRight(float value);
public:
	void KeyRoll();
	void KeyAttack();
	void PlayerAttack();
	void KeyCrouch();
	void KeyAssassination();
};
