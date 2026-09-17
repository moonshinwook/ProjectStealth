// Fill out your copyright notice in the Description page of Project Settings.
#include "Enemy.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"

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

	if (ChokeMontage)
	{
		const float Result = PlayAnimMontage(ChokeMontage);

		UE_LOG(LogTemp, Warning, TEXT("Choke Play Result: %f"), Result);

		//PlayAnimMontage(ChokeMontage);
	}
}


