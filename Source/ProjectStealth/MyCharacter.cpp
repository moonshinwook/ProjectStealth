// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "MyAnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
//	MotionWarping용 헤더파일
#include "MotionWarpingComponent.h"
//	HPBarWidget용 헤더파일
#include "HealthBarWidget.h"
#include "Components/WidgetComponent.h"

#include "Enemy.h"
//	블루프린트와 C++ 코드 모두에서 호출할 수 있는 유용한 게임플레이 유틸리티 함수들을 포함하는 정적 클래스
#include "Kismet/GameplayStatics.h"

#include "CharacterStateComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "GameFramework/DamageType.h"
#include "Components/InputComponent.h"
#include "InputCoreTypes.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM(TEXT("/Game/Man/Mesh/Full/SK_Man_Full_04.SK_Man_Full_04"));
	//	MotionWarpingComponet 생성자 추가.
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));

	StateComponent = CreateDefaultSubobject<UCharacterStateComponent>(TEXT("StateComponent"));

	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;

	if (SM.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SM.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FRotator(0.0f, -90.0f, 0.0f));
	}

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	SpringArm->SetupAttachment(RootComponent);
	Camera->SetupAttachment(SpringArm);

	SpringArm->TargetArmLength = 400.0f;
	SpringArm->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 100.0f), FRotator(-25.0f, 0.0f, 0.0f));
	SpringArm->bUsePawnControlRotation = true;

	HealthBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarComponent"));

	UHealthBarWidget::SetupHealthBarComponent(HealthBarComponent, RootComponent);


	static ConstructorHelpers::FClassFinder<UAnimInstance> ANI(TEXT("/Script/Engine.AnimBlueprint'/Game/BluePrints/ABP_MyCharacter.ABP_MyCharacter_C'"));
	if (ANI.Succeeded())
	{
		GetMesh()->SetAnimClass(ANI.Class);
	}

	//GetCapsuleComponent()->SetHiddenInGame(false);
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	//	캐릭터 상태 컴포넌트 유효성 검사 및 상태 출력
	if (IsValid(StateComponent))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Player: %s] LifeState = %s / DetectionState = %s"), *GetName(),
			StateComponent->IsAlive() ? TEXT("Alive") : TEXT("Dead"),
			StateComponent->IsDetected() ? TEXT("Detected") : TEXT("Hidden"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[Player: %s] StateComponent is invalid"), *GetName());
	}
	
	UpdateHealthUI();

	AnimInstance = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance());
}

//	HealthBar Update
void AMyCharacter::UpdateHealthUI()
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


void AMyCharacter::EnableRagdoll()
{
	if (GetMesh()->IsSimulatingPhysics())
	{
		return;
	}

	//	 이동 입력과 현재 이동 제거
	ConsumeMovementInputVector();
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();

	//	캐릭터 캡슐 충돌 해제
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//	메시를 물리 래그돌로 전환
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);

	UE_LOG(LogTemp, Warning, TEXT("Player Ragdoll 실행 : %s"), *GetName());
}

float AMyCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
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
	}

	return AppliedDamage;
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForwardBackward"), this, &AMyCharacter::KeyUpDown);
	PlayerInputComponent->BindAxis(TEXT("MoveLeftRight"), this, &AMyCharacter::KeyLeftRight);
	
	PlayerInputComponent->BindAxis(TEXT("LookUpDown"), this, &AMyCharacter::KeyLookUpDown);
	PlayerInputComponent->BindAxis(TEXT("LookLeftRight"), this, &AMyCharacter::KeyLookLeftRight);

	PlayerInputComponent->BindAction(TEXT("Roll"), EInputEvent::IE_Pressed, this, &AMyCharacter::KeyRoll);
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &AMyCharacter::KeyAttack);
	PlayerInputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Pressed, this, &AMyCharacter::KeyCrouch);
	PlayerInputComponent->BindAction(TEXT("Assassination"), EInputEvent::IE_Pressed, this, &AMyCharacter::KeyAssassination);	

	PlayerInputComponent->BindKey(
		EKeys::K,
		IE_Pressed,
		this,
		&AMyCharacter::TestSelfDamage);
}

void AMyCharacter::KeyUpDown(float value)
{
	//	암살 중에는 이동하지 않도록 처리
	if(bIsAssassinating)
	{
		return;
	}

	// 상태 컴포넌트가 없거나 사망했다면 이동 입력 무시
	if (!IsValid(StateComponent) || !StateComponent->IsAlive())
	{
		return;
	}

	AddMovementInput(GetActorForwardVector(), value, false);
}

void AMyCharacter::KeyLeftRight(float value)
{
	//	암살 중에는 이동하지 않도록 처리
	if (bIsAssassinating)
	{
		return;
	}

	// 상태 컴포넌트가 없거나 사망했다면 이동 입력 무시
	if (!IsValid(StateComponent) || !StateComponent->IsAlive())
	{
		return;
	}

	AddMovementInput(GetActorRightVector(), value, false);
}

void AMyCharacter::KeyLookUpDown(float value)
{
	AddControllerPitchInput(value);
}

void AMyCharacter::KeyLookLeftRight(float value)
{
	AddControllerYawInput(value);
}

void AMyCharacter::KeyRoll()
{
	//	암살 중에는 이동하지 않도록 처리
	if (bIsAssassinating)
	{
		return;
	}

	// 상태 컴포넌트가 없거나 사망했다면 이동 입력 무시
	if (!IsValid(StateComponent) || !StateComponent->IsAlive())
	{
		return;
	}

	if (IsValid(AnimInstance))
	{
		AnimInstance->PlayRollMontage();
	}
}

void AMyCharacter::KeyAttack()
{
	if (bIsAssassinating)
	{
		return;
	}

	// 상태 컴포넌트가 없거나 사망했다면 이동 입력 무시
	if (!IsValid(StateComponent) || !StateComponent->IsAlive())
	{
		return;
	}

	if (IsValid(AnimInstance))
	{
		AnimInstance->PlayAttackMontage();
	}
}

void AMyCharacter::PlayerAttack()
{
	// 상태 컴포넌트가 없거나 사망했다면 이동 입력 무시
	if (!IsValid(StateComponent) || !StateComponent->IsAlive())
	{
		return;
	}

	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);

	float AttackRange = 200.0f;
	float AttackRadius = 50.0f;
	float AttackHalfHeight = 90.0f;
	FVector StartPos = GetActorLocation();
	FVector EndPos = GetActorLocation() + GetActorForwardVector() * AttackRange;
	
	bool Result = GetWorld()->SweepSingleByChannel
	(
		HitResult, 
		StartPos, 
		EndPos, 
		FQuat::Identity, 
		ECC_GameTraceChannel1, // 채널 바꿈
		FCollisionShape::MakeCapsule(AttackRadius, AttackHalfHeight), 
		Params
	);

	// 공격방향
	FQuat AttackRotation = FRotationMatrix::MakeFromZ(EndPos).ToQuat();

	FColor DebugColor = Result ? FColor::Green : FColor::Red;

	FVector FwdVector = GetActorForwardVector() * AttackRange;

	FVector Center = StartPos + FwdVector * 0.5f;

	//DrawDebugCapsule
	//(
	//	GetWorld(), 
	//	StartPos, 
	//	AttackHalfHeight, 
	//	AttackRadius, 
	//	AttackRotation, 
	//	DebugColor, 
	//	false, 
	//	2.0f
	//);

	if (Result && HitResult.GetActor())
	{
		UE_LOG(LogTemp, Log, TEXT("Hit : %s"), *HitResult.GetActor()->GetName());
		AActor* Target = HitResult.GetActor();

		UGameplayStatics::ApplyDamage(Target, 25.0f, GetController(), this, NULL);
	}


}

void AMyCharacter::KeyCrouch()
{
	if (bIsAssassinating)
	{
		return;
	}

	// 상태 컴포넌트가 없거나 사망했다면 이동 입력 무시
	if (!IsValid(StateComponent) || !StateComponent->IsAlive())
	{
		return;
	}

	if (bIsCrouched)
	{
		UnCrouch();	// 이미 숙인 상태에서 C버튼 누르면 일어서기
	}
	else 
	{
		Crouch();	// 숙인 상태 아니면 숙이기 
	}
}



//	암살 중인지 확인하는 코드
void AMyCharacter::SetIsAssassinating(bool bNewIsAssassinating)
{

	bIsAssassinating = bNewIsAssassinating;

	if (bIsAssassinating)
	{
		//	암살 중이면 이번 프레임에 이미 들어온 이동 입력 제거
		ConsumeMovementInputVector();

		//	이동하던 속도 즉시 제거
		GetCharacterMovement()->StopMovementImmediately();
		
		//	암살 중에는 캐릭터가 회전하지 않도록 설정
		bUseControllerRotationYaw = false;

	}
}



void AMyCharacter::KeyAssassination()
{
	//	암살 중 재입력 제한
	if (bIsAssassinating)
	{
		return;
	}


	if (IsValid(AnimInstance))
	{
		AnimInstance->PlayAssassinationAttackMontage();

		//	Capsule Collision 해제
		//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	}

	//	현재 레벨에서 Enemy 한명 찾기
	AEnemy* Enemy = Cast<AEnemy>(UGameplayStatics::GetActorOfClass(GetWorld(), AEnemy::StaticClass()));

	UE_LOG(LogTemp, Log, TEXT("Found Enemy : %s"), *GetNameSafe(Enemy));

	//	적이 없으면 반환
	if (!IsValid(Enemy))
	{
		UE_LOG(LogTemp, Warning, TEXT("Assassination: Enemy not found"));
		return;
	}



	//	Enemy가 유효하면 Enemy의 PlayChoke() 함수 호출
	if (IsValid(Enemy))
	{
		//	Enemy의 암살 기준점 정보 가져오기.
		const FTransform TargetTransform = Enemy->GetAssassinationTransform();

		//	몽타주 재생 전에 워핑 목표 등록
		MotionWarpingComponent->AddOrUpdateWarpTargetFromTransform(FName(TEXT("AssassinationTarget")), TargetTransform);

		//	목표 위치 방향 확인
		UE_LOG(LogTemp, Log, TEXT("Assassination Target : %s / Location : %s / Rotation : %s"),
			*GetNameSafe(Enemy), *TargetTransform.GetLocation().ToString(), *TargetTransform.Rotator().ToString());
		
		//	플레이어 Motion Warping 동안 대상 적과의 충돌 무시.
		GetCapsuleComponent()->IgnoreActorWhenMoving(Enemy, true);
		// 대상 적이 이동할 때 플레이어와의 충돌 무시
		Enemy->GetCapsuleComponent()->IgnoreActorWhenMoving(this, true);

		Enemy->PlayChoke();
	}


}

void AMyCharacter::TestSelfDamage()
{
	UE_LOG(LogTemp, Warning, TEXT("Player 피해 테스트: K 입력"));

	UGameplayStatics::ApplyDamage(
		this,                       // 피해를 받는 대상: 자신
		25.0f,                      // 피해량
		GetController(),            // 피해를 일으킨 컨트롤러
		this,                       // 피해를 일으킨 Actor
		UDamageType::StaticClass()  // 기본 피해 유형
	);
}

void AMyCharacter::SetIsAssassinatingEnd()
{
	//	암살이 끝나면 캐릭터의 회전 관련 설정을 원래대로 복원
	bUseControllerRotationYaw = true;
}