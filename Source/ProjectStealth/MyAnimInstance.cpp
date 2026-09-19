// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MyCharacter.h"



UMyAnimInstance::UMyAnimInstance()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> RollMontageFinder(TEXT("/Game/Animaitions/Montage/RM_Roll_front1_Montage.RM_Roll_front1_Montage"));
	
	
	if (RollMontageFinder.Succeeded())
	{
		RollMontage = RollMontageFinder.Object;
	}

	
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AttackMontageFinder(TEXT("/Game/Animaitions/Montage/anim_Counter_Attack_01_Montage.anim_Counter_Attack_01_Montage"));


	if (AttackMontageFinder.Succeeded())
	{
		AttackMontage = AttackMontageFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> AssassinationAttackMontageFinder(TEXT("/Game/Animaitions/Montage/AM_Assassination_Animation_Montage.AM_Assassination_Animation_Montage"));

	if (AssassinationAttackMontageFinder.Succeeded())
	{
		AssassinationAttackMontage = AssassinationAttackMontageFinder.Object;
	}
}


void UMyAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	APawn* Pawn = TryGetPawnOwner();
	 
	if (IsValid(Pawn))
	{
		Character = Cast<AMyCharacter>(Pawn);

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

		bIsCrouched = CharacterMovement->IsCrouching();
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

			Character->PlayerAttack();
		}
	}
}

void UMyAnimInstance::PlayAssassinationAttackMontage()
{
	//	기존 코드 -> 롤백용
	//if (IsValid(AssassinationAttackMontage))
	//{
	//	if (!Montage_IsPlaying(AssassinationAttackMontage))
	//	{
	//		Montage_Play(AssassinationAttackMontage, 1.0f);
	//	}
	//}
	if (!IsValid(AssassinationAttackMontage) || !IsValid(Character))
	{
		return;
	}

	// 블렌드 아웃 중인 경우를 포함해 중복 재생 방지
	if (Montage_IsActive(AssassinationAttackMontage))
	{
		return;
	}

	const float PlayResult =
		Montage_Play(AssassinationAttackMontage, 1.0f);

	// 재생 실패 시 이동을 막지 않음
	if (PlayResult <= 0.0f)
	{
		return;
	}

	// 재생 성공 → WASD 이동 제한
	Character->SetIsAssassinating(true);

	// 이 암살 몽타주가 끝나면 호출할 함수 연결
	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(
		this,
		&UMyAnimInstance::OnAssassinationMontageEnded
	);

	Montage_SetEndDelegate(EndDelegate,AssassinationAttackMontage);
}

void UMyAnimInstance::OnAssassinationMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != AssassinationAttackMontage)
	{
		return;
	}

	if (IsValid(Character))
	{
		// 정상 종료와 중간에 끊긴 경우 모두 이동 제한 해제
		Character->SetIsAssassinating(false);
	}
}
