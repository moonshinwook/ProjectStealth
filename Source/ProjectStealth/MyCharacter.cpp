// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "MyAnimInstance.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<
		
		USkeletalMesh> SM(TEXT("/Game/Man/Mesh/Full/SK_Man_Full_04.SK_Man_Full_04"));

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

	static ConstructorHelpers::FClassFinder<UAnimInstance> ANI(TEXT("/Script/Engine.AnimBlueprint'/Game/BluePrints/ABP_MyCharacter.ABP_MyCharacter_C'"));
	//static ConstructorHelpers::FClassFinder<UAnimInstance> ANI(TEXT("/Script/Engine.AnimBlueprint'/Game/BluePrints/ABP_MyCharacter.ABP_MyCharacter_C'"));
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
	PlayerInputComponent->BindAction(TEXT("Roll"), EInputEvent::IE_Pressed, this, &AMyCharacter::KeyRoll);
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &AMyCharacter::KeyAttack);

}

void AMyCharacter::KeyUpDown(float value)
{
	AddMovementInput(GetActorForwardVector(), value, false);
}

void AMyCharacter::KeyLeftRight(float value)
{
	AddMovementInput(GetActorRightVector(), value, false);
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

