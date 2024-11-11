#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "DemoAnimInstance.generated.h"

class ADemoCharacter;

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
	TObjectPtr<ADemoCharacter> DemoCharacter; // Character AnimInstance controls
	
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float VelocityZ; // Character Z-axis velocity
	
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float Speed; // Character XY-axis speed
	
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsInAir; // Character in air 
	
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating; // Character has some key pressed on

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bWeaponEquipped; // Character has a weapon equipped
};