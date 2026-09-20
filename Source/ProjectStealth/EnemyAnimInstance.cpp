// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"


void UEnemyAnimInstance::AnimNotify_StartRagdoll()
{
	AEnemy* Enemy = Cast<AEnemy>(TryGetPawnOwner());

	if (IsValid(Enemy))
	{
		Enemy->EnableRagdoll();
	}
}
