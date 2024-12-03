#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"
#include "RunSlideMovementComponent.generated.h"

class ADemoCharacter;

/**
 * 
 */
UCLASS()
class DEMO_API URunSlideMovementComponent : public UMovementComponent
{
	GENERATED_BODY()

public:
	URunSlideMovementComponent();
	// DemoCharacter can access private members of RunSlideMovementComponent
	friend class ADemoCharacter;
	// Run-slide duration
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movements")
	float SlideDuration;
	// MaxSlideSpeed = MaxWalkSpeed * SlideSpeedMultiplier
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movements")
	float SlideSpeedMultiplier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movements")
	float OriginalMaxWalkSpeed;
	// Play stop animation
	bool bIsSliding;
	
	// Initiate slide, callable from character
	void InitiateSlide();
	// Determine if the character can slide
	bool CanSlide();
	// RPC, executed on server
	UFUNCTION(Server, Reliable)
	void Server_InitiateSlide();
	// Multicast
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_InitiateSlide();
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	// Timer handle for slide duration
	FTimerHandle SlideTimerHandle;
	
	// Slide functions
	void StartSlide();
	void StopSlide();
	
private:
	ADemoCharacter* Character;
};
