// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "MyAnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Enemy.h"
//	블루프린트와 C++ 코드 모두에서 호출할 수 있는 유용한 게임플레이 유틸리티 함수들을 포함하는 정적 클래스
#include "Kismet/GameplayStatics.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM(TEXT("/Game/Man/Mesh/Full/SK_Man_Full_04.SK_Man_Full_04"));

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

	static ConstructorHelpers::FClassFinder<UAnimInstance> ANI(TEXT("/Script/Engine.AnimBlueprint'/Game/BluePrints/ABP_MyCharacter.ABP_MyCharacter_C'"));
	if (ANI.Succeeded())
	{
		GetMesh()->SetAnimClass(ANI.Class);
	}

	GetCapsuleComponent()->SetHiddenInGame(false);
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	AnimInstance = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance());
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
}

void AMyCharacter::KeyUpDown(float value)
{
	//	암살 중에는 이동하지 않도록 처리
	if(bIsAssassinating)
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

	if (IsValid(AnimInstance))
	{
		AnimInstance->PlayAttackMontage();
	}
}

void AMyCharacter::PlayerAttack()
{
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

	DrawDebugCapsule
	(
		GetWorld(), 
		StartPos, 
		AttackHalfHeight, 
		AttackRadius, 
		AttackRotation, 
		DebugColor, 
		false, 
		2.0f
	);

	if (Result)
	{
		UE_LOG(LogTemp, Log, TEXT("Hit : %s"), *HitResult.GetActor()->GetName());
	}

}

void AMyCharacter::KeyCrouch()
{
	if (bIsAssassinating)
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
		bUseControllerRotationPitch = false;
		bUseControllerRotationRoll = false;

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

	//	Enemy가 유효하면 Enemy의 PlayChoke() 함수 호출
	if (IsValid(Enemy))
	{
		Enemy->PlayChoke();
	}

}