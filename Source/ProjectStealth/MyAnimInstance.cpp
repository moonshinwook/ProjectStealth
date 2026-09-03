// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"



UMyAnimInstance::UMyAnimInstance()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AM(TEXT("/Script/Engine.AnimMontage'/Game/Animaitions/RM_Roll_front1_Montage.RM_Roll_front1_Montage'"));
	
	
	if (AM.Succeeded())
	{
		RollMontage = AM.Object;
	}

	
		static ConstructorHelpers::FObjectFinder<UAnimMontage> AAM(TEXT("/Script/Engine.AnimMontage'/Game/Animaitions/uppercut_right_Anim_Montage.uppercut_right_Anim_Montage'"));


		if (AAM.Succeeded())
		{
			AttackMontage = AAM.Object;
		}

}

void UMyAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	APawn* Pawn = TryGetPawnOwner();
	 
	if (IsValid(Pawn))
	{
		Character = Cast<ACharacter>(Pawn);

		if (IsValid(Character))
		{
			CharacterMovement = Character->GetCharacterMovement();
		}
	}
}

void UMyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (IsValid(CharacterMovement))
	{
		FVector Velocity = CharacterMovement->Velocity;
		float GroundSpeed = Velocity.Size2D();
		FRotator Rotation = Character->GetActorRotation();
		FVector UnrotateVector = Rotation.UnrotateVector(Velocity);
		UnrotateVector.Normalize();

		Vertical = UnrotateVector.X;
		Horizontal = UnrotateVector.Y;
		

		auto Acceleration = CharacterMovement->GetCurrentAcceleration();

		ShouldMove = GroundSpeed > 0.1 && Acceleration != FVector::Zero();
	}
}

void UMyAnimInstance::PlayRollMontage()
{
	if (IsValid(RollMontage))
	{
		if (!Montage_IsPlaying(RollMontage))
		{
			Montage_Play(RollMontage, 1.0f);
		}
	}
}

void UMyAnimInstance::PlayAttackMontage()
{
	if(IsValid(AttackMontage))
	{
		if (!Montage_IsPlaying(AttackMontage))
		{
			Montage_Play(AttackMontage, 1.0f);
		}
	}
}
