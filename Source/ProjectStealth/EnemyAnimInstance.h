// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Enemy.h"
#include "CoreMinimal.h"
#include "MyAnimInstance.h"
#include "EnemyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSTEALTH_API UEnemyAnimInstance : public UMyAnimInstance
{
	GENERATED_BODY()
public:
	UFUNCTION()
	void AnimNotify_StartRagdoll();
};
