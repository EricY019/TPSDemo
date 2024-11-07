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
	// The character AnimInstance controls
	UPROPERTY(BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ADemoCharacter> DemoCharacter;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float VelocityZ;
	
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;

	// If some key is pressed on
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;
};
