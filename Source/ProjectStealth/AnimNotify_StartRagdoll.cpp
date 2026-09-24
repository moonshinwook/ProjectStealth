// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_StartRagdoll.h"
#include "Components/SkeletalMeshComponent.h"
#include "Enemy.h"

void UAnimNotify_StartRagdoll::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	UE_LOG(LogTemp, Warning, TEXT("Ragdoll Notify ENTER"));
	Super::Notify(MeshComp, Animation, EventReference);

	if (!IsValid(MeshComp))
	{
		return;
	}

	// 이 애니메이션을 재생하는 메시의 소유자를 Enemy로 확인
	AEnemy* Enemy = Cast<AEnemy>(MeshComp->GetOwner());

	if (!IsValid(Enemy))
	{
		return;
	}

	UE_LOG(LogTemp, Warning,
		TEXT("StartRagdoll Notify / Enemy: %s"),
		*GetNameSafe(Enemy));

	Enemy->EnableRagdoll();
}
