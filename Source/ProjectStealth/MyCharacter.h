// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "MyCharacter.generated.h"


class UWidgetComponent;
class UMyAnimInstance;
class AEnemy;  
class UMotionWarpingComponent;
class UCharacterStateComponent;

UCLASS()
class PROJECTSTEALTH_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()
private:
	UPROPERTY(VisibleAnywhere)
	class UMyAnimInstance* AnimInstance;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Player|Assassination",meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent;
protected:
	//	최대 체력
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|State")
	float MaxHealth = 100.0f;
	//	현재 체력
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|State")
	float CurrentHealth = 100.0f;
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

public:
	// 현재 체력을 0~1 비율로 반환
	UFUNCTION(BlueprintPure, Category = "Character|Health")
	float GetHealthPercent() const;
	void UpdateHealthUI();
private:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Character|UI",meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> HealthBarComponent;


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

private:
	UPROPERTY(VisibleAnywhere, Category = "State")
	TObjectPtr<UCharacterStateComponent> StateComponent;

public:
	void EnableRagdoll();
public:
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
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


private:
	void TestSelfDamage();

};
