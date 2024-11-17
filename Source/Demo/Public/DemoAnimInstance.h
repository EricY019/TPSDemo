#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Demo/Public/CharacterTypes/TurningInPlace.h"
#include "DemoAnimInstance.generated.h"

class ADemoCharacter;
class AWeapon;

UCLASS()
class DEMO_API UDemoAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	// Called at initializing animation
	virtual void NativeInitializeAnimation() override;
	// Called at every frame
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY(BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ADemoCharacter> DemoCharacter; // character AnimInstance controls

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float VelocityZ; // character Z-axis velocity

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float Speed; // character XY-axis speed

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsInAir; // character in air 

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating; // character has some key pressed on

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bWeaponEquipped; // has a weapon equipped
	
	AWeapon* EquippedWeapon; // character equipped weapon
	
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bAiming; // character is aiming

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float YawOffset; // character yaw

	FRotator DeltaRotation; // interpolate strafing

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float AO_Yaw; // yaw to aim offset

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float AO_Pitch; // pitch to aim offset

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	FTransform LeftHandTransform; // left hand transform 

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	ETurningInPlace TurningInPlace; // turning in place
};
