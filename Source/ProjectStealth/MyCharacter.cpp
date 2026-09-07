// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "MyAnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

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

}

void AMyCharacter::KeyUpDown(float value)
{
	AddMovementInput(GetActorForwardVector(), value, false);
}

void AMyCharacter::KeyLeftRight(float value)
{
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
	if (IsValid(AnimInstance))
	{
		AnimInstance->PlayRollMontage();
	}
}

void AMyCharacter::KeyAttack()
{
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
	if (bIsCrouched)
	{
		UnCrouch();	// 이미 숙인 상태에서 C버튼 누르면 일어서기
	}
	else 
	{
		Crouch();	// 숙인 상태 아니면 숙이기 
	}
}



