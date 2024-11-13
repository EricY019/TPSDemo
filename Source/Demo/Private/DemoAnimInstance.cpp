#include "DemoAnimInstance.h"
#include "DemoCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

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
	
	// Update character movements
	FVector Velocity = DemoCharacter->GetVelocity();
	VelocityZ = Velocity.Z;
	Velocity.Z = 0.f;
	Speed = Velocity.Size();
	bIsInAir = DemoCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = DemoCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f;
	bWeaponEquipped = DemoCharacter->IsWeaponEquipped();
	bAiming = DemoCharacter->IsAiming();
	// Update character yaw
	FRotator AimRotation = DemoCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(Velocity);
	FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, Delta, DeltaTime, 6.f);
	YawOffset = DeltaRotation.Yaw;
	// Update aiming offset
	AO_Yaw = DemoCharacter->GetAOYaw();
	AO_Pitch = DemoCharacter->GetAOPitch();
}
