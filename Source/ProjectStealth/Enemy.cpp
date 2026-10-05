
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
//	HealthBarWidget 헤더파일
#include "HealthBarWidget.h"
#include "Components/WidgetComponent.h"

#include "Kismet/GameplayStatics.h"

#include "CharacterStateComponent.h"



// Sets default values, 생성자.
AEnemy::AEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Enemy의 캡슐이 카메라를 막지 않도록 설정
	GetCapsuleComponent()->SetCollisionResponseToChannel(
		ECC_Camera, ECR_Ignore);

	// Enemy의 메시가 카메라를 막지 않도록 설정
	GetMesh()->SetCollisionResponseToChannel(
		ECC_Camera, ECR_Ignore);

	StateComponent = CreateDefaultSubobject<UCharacterStateComponent>(TEXT("StateComponent"));

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
	//AssassinationPoint->SetHiddenInGame(false);

	HealthBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarComponent"));

	UHealthBarWidget::SetupHealthBarComponent(HealthBarComponent, RootComponent);
}


// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;


	//	캐릭터 상태 컴포넌트 유효성 검사 및 상태 출력
	if (IsValid(StateComponent))
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[Player: %s] LifeState = %s / DetectionState = %s"),
			*GetName(),
			StateComponent->IsAlive() ? TEXT("Alive") : TEXT("Dead"),
			StateComponent->IsDetected() ? TEXT("Detected") : TEXT("Hidden"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[Player: %s] StateComponent is invalid"),
			*GetName());
	}
	
}

FTransform AEnemy::GetAssassinationTransform() const
{
	return AssassinationPoint->GetComponentTransform();
}

bool AEnemy::IsAlive() const
{
	return IsValid(StateComponent) && StateComponent->IsAlive();
}

//	사망 처리 표현 코드
void AEnemy::EnableRagdoll()
{
	// 이미 래그돌 상태라면 중복 실행 방지
	if (GetMesh()->IsSimulatingPhysics())
	{
		return;
	}
	
	// 캐릭터 이동 중단
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();

	// 캡슐 충돌 해제
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 몸에 래그돌용 충돌 설정 적용
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));

	// Player 캡슐이 사용하는 Pawn 채널과 충돌하지 않도록 설정
	GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	// 중력과 물리 시뮬레이션 활성화
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetSimulatePhysics(true);

	UE_LOG(LogTemp, Warning, TEXT("Ragdoll / IsSimulatingPhysics: %s"), GetMesh()->IsSimulatingPhysics() ? TEXT("true") : TEXT("false"));
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

float AEnemy::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!IsValid(StateComponent))
	{
		return 0.0f;
	}

	// 이미 사망한 상태라면 추가 피해 및 사망 처리 방지
	if (!StateComponent->IsAlive())
	{
		return 0.0f;
	}

	// 체력 감소 및 생존 → 사망 상태 변경
	const float AppliedDamage =
		StateComponent->ReceiveDamage(Damage);

	// 변경된 체력으로 HP바 갱신
	UpdateHealthUI();

	// 이번 피해로 체력이 소진되었다면 래그돌 실행
	if (StateComponent->GetCurrentHealth() <= 0.0f)
	{
		EnableRagdoll();
		StateComponent->MarkDead();
	}

	return AppliedDamage;
}

void AEnemy::UpdateHealthUI()
{
	// 상태 컴포넌트와 HP바 컴포넌트 확인
	if (!IsValid(StateComponent) || !IsValid(HealthBarComponent))
	{
		return;
	}

	UHealthBarWidget* HealthWidget =
		Cast<UHealthBarWidget>(HealthBarComponent->GetUserWidgetObject());

	if (!IsValid(HealthWidget))
	{
		return;
	}

	// 컴포넌트의 체력으로 HP바 갱신
	HealthWidget->UpdateHealth(StateComponent->GetCurrentHealth(), StateComponent->GetMaxHealth());
}

void AEnemy::PlayChoke()
{
	UE_LOG(LogTemp, Warning, TEXT("PlayChoke Called / Montage: %s"), *GetNameSafe(ChokeMontage));

	if (ChokeMontage)
	{
		const float Result = PlayAnimMontage(ChokeMontage);

		UE_LOG(LogTemp, Warning, TEXT("Choke Play Result: %f"), Result);
	}


}

void AEnemy::CompleteAssassination()
{
	if (!StateComponent->IsAlive())
	{
		return;
	}

	// 남은 체력만큼 피해를 적용하여 체력 0 및 dead 상태로 변경
	const float RemainHealth = StateComponent->GetCurrentHealth();

	StateComponent->ReceiveDamage(RemainHealth);

	//  HP바 갱신
	UpdateHealthUI();

	UE_LOG(LogTemp, Warning, TEXT("[%s] 암살 완료 / Health: %.1f / Alive: %s"),
		*GetName(),
		StateComponent->GetCurrentHealth(),
		StateComponent->IsAlive() ? TEXT("true") : TEXT("false"));
}

bool AEnemy::IsTargetInAssassinationAngle(const AActor* Target) const
{
	if (!IsValid(Target))
	{
		return false;
	}


	//  Enemy가 바라보는 방향 : 높이를 제외하고 정규화
	const FVector EnemyForward = GetActorForwardVector().GetSafeNormal2D();

	//	Enemy에서 Target으로 향하는 방향 : 높이를 제외하고 정규화
	const FVector DirectionToTarget = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();

	//  수평 위치가 겹치는 등 방향을 구할 수 없는 경우 제외
	if (EnemyForward.IsNearlyZero() || DirectionToTarget.IsNearlyZero())
	{
		return false;
	}

	//	정규화된 두 방향의 내적 벡터값
	const double Dot = FVector::DotProduct(EnemyForward, DirectionToTarget);

	//	전방 전체 140도의 절반인 70도를 기준으로 비교
	const double FrontThreshold = FMath::Cos(FMath::DegreesToRadians(70.0));

	//  경계 포함 전방은 불허, 나머지 영역은 허용
	const bool bInAssassinationAngle = (Dot < FrontThreshold);

	UE_LOG(LogTemp, Log, TEXT("Enemy : %s / Dot : %.6f / AngleAllowed : %s"), *GetName(), Dot, bInAssassinationAngle ? TEXT("true") : TEXT("false"));

	return bInAssassinationAngle;
}


