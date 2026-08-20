// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


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
