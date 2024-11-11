// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoAnimInstance.h"
#include "DemoCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UDemoAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	DemoCharacter = Cast<ADemoCharacter>(TryGetPawnOwner());
}

void UDemoAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	// Case for accessing character before init, safely return
	if (DemoCharacter == nullptr)
	{
		DemoCharacter = Cast<ADemoCharacter>(TryGetPawnOwner());
	}
	if (DemoCharacter == nullptr) return;
	
	// Update velocityZ, speed, bIsInAir, bIsAccelerating
	FVector Velocity = DemoCharacter->GetVelocity();
	VelocityZ = Velocity.Z;
	Velocity.Z = 0.f;
	Speed = Velocity.Size();
	bIsInAir = DemoCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = DemoCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f;

	// Update bWeaponEquipped(
	bWeaponEquipped = DemoCharacter->IsWeaponEquipped();
}
