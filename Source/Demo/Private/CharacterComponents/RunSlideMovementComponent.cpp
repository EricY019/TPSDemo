#include "CharacterComponents/RunSlideMovementComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DemoCharacter.h"
#include "Net/UnrealNetwork.h"

URunSlideMovementComponent::URunSlideMovementComponent()
{
	// Init default
	bIsSliding = false;
	SlideDuration = 0.9f;
	SlideSpeedMultiplier = 1.2f;
	
	SetIsReplicatedByDefault(true); // replicating component
}

void URunSlideMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	Character = Cast<ADemoCharacter>(GetOwner());
	if (Character && Character->GetCharacterMovement())
	{
		OriginalMaxWalkSpeed = Character->GetCharacterMovement()->MaxWalkSpeed;
	}
}

bool URunSlideMovementComponent::CanSlide()
{
	// Return false if movement component has not been constructed
	if (!Character)
	{
		return false;
	}
	// Return false if sliding, moving slowly, or is in air
	if (bIsSliding || Character->GetVelocity().Size() < 100.f || Character->GetCharacterMovement()->IsFalling())
	{
		return false;
	}
	return true;
}

void URunSlideMovementComponent::InitiateSlide()
{
	if (!CanSlide()) return;
	
	StartSlide();
	// RPC to server
	Server_InitiateSlide();
}

void URunSlideMovementComponent::Server_InitiateSlide_Implementation()
{
	Multicast_InitiateSlide();
}

void URunSlideMovementComponent::Multicast_InitiateSlide_Implementation()
{
	if (Character && Character->IsLocallyControlled()) return;
	StartSlide();
}

void URunSlideMovementComponent::StartSlide()
{
	// Play sliding anim
	bIsSliding = true;
	// Adjust max walk speed
	Character->GetCharacterMovement()->MaxWalkSpeed *= SlideSpeedMultiplier;
	// Set timer to stop slide after duration
	if (Character && Character->GetWorld())
	{
		Character->GetWorld()->GetTimerManager().SetTimer(SlideTimerHandle, this,
			&URunSlideMovementComponent::StopSlide, SlideDuration, false);
	}
}

void URunSlideMovementComponent::StopSlide()
{
	// Stop sliding anim
	bIsSliding = false;
	
	// Revert max walk speed
	Character->GetCharacterMovement()->MaxWalkSpeed = OriginalMaxWalkSpeed;

	// Clear timer
	if (Character && Character->GetWorld())
	{
		Character->GetWorld()->GetTimerManager().ClearTimer(SlideTimerHandle);
	}
}



