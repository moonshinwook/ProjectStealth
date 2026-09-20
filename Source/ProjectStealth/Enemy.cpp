
// Fill out your copyright notice in the Description page of Project Settings.
#include "Enemy.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "MyCharacter.h"

// Sets default values
AEnemy::AEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CurrentHealth = MaxHealth;
}


// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
}

void AEnemy::OnChokeMontageEnded(UAnimMontage* Montage, bool bInterrupted)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("Choke Ended / Interrupted: %s"),
			bInterrupted ? TEXT("true") : TEXT("false"));

		// 중단된 경우는 제외하고, 정상 종료했을 때만 실행
		if (!bInterrupted)
		{
			EnableRagdoll();
		}
}

//	사망 처리 표현 코드
void AEnemy::EnableRagdoll()
{
	// 캐릭터 이동 중단
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();

	// 캡슐 충돌 해제
	GetCapsuleComponent()->SetCollisionEnabled(
		ECollisionEnabled::NoCollision
	);

	// 몸에 래그돌용 충돌 설정 적용
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));

	// 중력과 물리 시뮬레이션 활성화
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetSimulatePhysics(true);


	UE_LOG(LogTemp, Warning,
		TEXT("Ragdoll / IsSimulatingPhysics: %s"),
		GetMesh()->IsSimulatingPhysics() ? TEXT("true") : TEXT("false"));
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::PlayChoke()
{
	UE_LOG(LogTemp, Warning, TEXT("PlayChoke Called / Montage: %s"), *GetNameSafe(ChokeMontage));

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (ChokeMontage)
	{
		const float Result = PlayAnimMontage(ChokeMontage);

		UE_LOG(LogTemp, Warning, TEXT("Choke Play Result: %f"), Result);

		// Enemy를 월드 좌표 X=0, Y=0, Z=0으로 이동
		SetActorLocation(FVector(80.0f, 0.0f, 90.0f));

		if (Result > 0.0f)
		{
			if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
			{
				FOnMontageEnded EndDelegate;
				EndDelegate.BindUObject(this, &AEnemy::OnChokeMontageEnded);

				AnimInstance->Montage_SetEndDelegate(
					EndDelegate, ChokeMontage
				);
			}
		}

	}

}


