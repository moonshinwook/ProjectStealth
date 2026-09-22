// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"


// 전방 선언
class UAnimMonatge;

UCLASS()
class PROJECTSTEALTH_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
protected:
	//	최대 체력
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|State")
	float MaxHealth = 100.0f;
	//	현재 체력
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|State")
	float CurrentHealth = 100.0f;
	//	사망 처리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|State")
	bool bIsDead = false;
	//	피암살 처리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|State")
	bool bIsBeingAssassinated = false;
	//	사망 처리
private:
	void OnChokeMontageEnded(UAnimMontage* Montage, bool bInterrupted);
public:
	UFUNCTION(BlueprintCallable, Category = "Enemy|Death")
	void EnableRagdoll();

protected:
	//	적이 초크를 당하는 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Animation")
	UAnimMontage* ChokeMontage = nullptr;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
public:
	void PlayChoke();
};
