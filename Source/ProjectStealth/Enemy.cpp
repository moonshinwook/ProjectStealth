
// Fill out your copyright notice in the Description page of Project Settings.
#include "Enemy.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
//	화살표 표시를 위한 헤더파일
#include "Components/ArrowComponent.h"
#include "MyCharacter.h"


// Sets default values, 생성자.
AEnemy::AEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CurrentHealth = MaxHealth;
	//	암살 포인트를 나타내는 화살표 컴포넌트 생성, 위치 방향 표시
	AssassinationPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("AssassinationPoint"));
	//	Enemy의 루트 컴포넌트에 부착
	AssassinationPoint->SetupAttachment(GetRootComponent());
	//	임시 기준값 : Enemy 기준 뒤쪽 80cm
	AssassinationPoint->SetRelativeLocation(FVector(-35.0f, 0.0f, 0.0f));
	//	Enemy와 같은 방향
	AssassinationPoint->SetRelativeRotation(FRotator::ZeroRotator);
	//	기존 화살표와 구분하기 위한 색상
	AssassinationPoint->SetArrowColor(FColor::Green);
	//	게임 실행 중에 화살표 숨김 해제, 육안으로 확인용
	AssassinationPoint->SetHiddenInGame(false);
}


// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
}

FTransform AEnemy::GetAssassinationTransform() const
{
	return AssassinationPoint->GetComponentTransform();
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

	// Player 캡슐이 사용하는 Pawn 채널과 충돌하지 않도록 설정
	GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

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

	//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (ChokeMontage)
	{
		const float Result = PlayAnimMontage(ChokeMontage);

		UE_LOG(LogTemp, Warning, TEXT("Choke Play Result: %f"), Result);

		// Enemy를 월드 좌표 X=0, Y=0, Z=0으로 이동
		//SetActorLocation(FVector(80.0f, 0.0f, 90.0f));

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


