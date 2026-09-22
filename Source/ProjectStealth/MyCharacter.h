// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "MyCharacter.generated.h"



class UMyAnimInstance;
class AEnemy;  
class UMotionWarpingComponent;

UCLASS()
class PROJECTSTEALTH_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()
private:
	UPROPERTY(VisibleAnywhere)
	class UMyAnimInstance* AnimInstance;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Player|Assassination",meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent;

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

//	암살관련 정의
public:
	void SetIsAssassinating(bool bNewIsAssassinating);
	void SetIsAssassinatingEnd();
private:
	UPROPERTY(VisibleAnywhere, Category = "Player|Assassination")
	bool bIsAssassinating = false;


private:
	//	캐릭터의 회전 관련 설정을 저장하기 위한 변수
	bool bSavedUseControllerRotationYaw = false;

	
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
