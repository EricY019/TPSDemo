#include "CharacterComponents/DemoCharacterMovementComponent.h"
#include "DemoCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

bool UDemoCharacterMovementComponent::FSavedMove_Demo::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const
{
	// Return false if sprint cannot be combined
	FSavedMove_Demo* NewDemoMove = static_cast<FSavedMove_Demo*>(NewMove.Get());
	if (Saved_bWantsToSprint != NewDemoMove->Saved_bWantsToSprint)
	{
		return false;
	}
	
	return FSavedMove_Character::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void UDemoCharacterMovementComponent::FSavedMove_Demo::Clear()
{
	// Reset saved move object
	FSavedMove_Character::Clear();

	Saved_bWantsToSprint = 0;
}

uint8 UDemoCharacterMovementComponent::FSavedMove_Demo::GetCompressedFlags() const
{
	// Send compressed flags to replicate data
	uint8 Result = Super::GetCompressedFlags();
	if (Saved_bWantsToSprint)
	{
		Result = FLAG_Custom_0; // custom flag
	}
	return Result;
}

void UDemoCharacterMovementComponent::FSavedMove_Demo::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	// Capture the safe data, send to saved data
	FSavedMove_Character::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);

	UDemoCharacterMovementComponent* CharacterMovement = Cast<UDemoCharacterMovementComponent>(C->GetCharacterMovement());

	Saved_bWantsToSprint = CharacterMovement->Safe_bWantsToSprint;
}

void UDemoCharacterMovementComponent::FSavedMove_Demo::PrepMoveFor(ACharacter* C)
{
	// Capture the saved data, send to safe data
	FSavedMove_Character::PrepMoveFor(C);

	UDemoCharacterMovementComponent* CharacterMovement = Cast<UDemoCharacterMovementComponent>(C->GetCharacterMovement());

	CharacterMovement->Safe_bWantsToSprint = Saved_bWantsToSprint;
}

UDemoCharacterMovementComponent::FNetworkPredictionData_Client_Demo::FNetworkPredictionData_Client_Demo(const UCharacterMovementComponent& ClientMovement) : Super(ClientMovement)
{
}

FSavedMovePtr UDemoCharacterMovementComponent::FNetworkPredictionData_Client_Demo::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_Demo());
}

FNetworkPredictionData_Client* UDemoCharacterMovementComponent::GetPredictionData_Client() const
{
	check(PawnOwner != nullptr);

	if (ClientPredictionData == nullptr)
	{
		UDemoCharacterMovementComponent* MutableThis = const_cast<UDemoCharacterMovementComponent*>(this);

		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_Demo(*this);
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
	}
	return ClientPredictionData;
}

void UDemoCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	Safe_bWantsToSprint = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
}

UDemoCharacterMovementComponent::UDemoCharacterMovementComponent()
{
}

void UDemoCharacterMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();

	DemoCharacterOwner = Cast<ADemoCharacter>(GetOwner());
}

void UDemoCharacterMovementComponent::SprintPressed()
{
	Safe_bWantsToSprint = true;
}

void UDemoCharacterMovementComponent::SprintReleased()
{
	Safe_bWantsToSprint = false;
}

void UDemoCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation,
	const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);

	if (MovementMode == MOVE_Walking)
	{
		if (Safe_bWantsToSprint)
		{
			MaxWalkSpeed = Sprint_MaxWalkSpeed;
		}
		else
		{
			MaxWalkSpeed = Walk_MaxWalkSpeed;
		}
	}
}

void UDemoCharacterMovementComponent::EnterSlide()
{
	bWantsToCrouch = true; // shorten capsule component - utilize crouching
	Velocity += Velocity.GetSafeNormal2D() * Slide_EnterImpulse;
	SetMovementMode(MOVE_Custom, CMOVE_Slide);
}

void UDemoCharacterMovementComponent::ExitSlide()
{
	bWantsToCrouch = false;

	// Correct rotation
	FQuat NewRotation = FRotationMatrix::MakeFromXZ(UpdatedComponent->GetForwardVector().GetSafeNormal2D(), FVector::UpVector).ToQuat();
	FHitResult Hit;
	SafeMoveUpdatedComponent(FVector::ZeroVector, NewRotation, true, Hit);
	SetMovementMode(MOVE_Walking);
}

bool UDemoCharacterMovementComponent::GetSlideSurface(FHitResult& Hit) const
{
	FVector Start = UpdatedComponent->GetComponentLocation();
	FVector End = Start + CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.5f * FVector::DownVector;
	FName ProfileName = TEXT("BlockAll");
	return GetWorld()->LineTraceTestByProfile(Start, End, ProfileName, DemoCharacterOwner->GetIgnoreCharacterParams());
}

void UDemoCharacterMovementComponent::PhysSlide(float DeltaTime, int32 Iterations)
{
	if (DeltaTime < MIN_TICK_TIME) return;
	
	RestorePreAdditiveRootMotionVelocity();

	// Determine on surface
	FHitResult SurfaceHit;
	if (!GetSlideSurface(SurfaceHit) || Velocity.SizeSquared() < pow(Slide_MinSpeed, 2))
	{
		// Exit slide if not on the surface, or current speed is less than min speed
		ExitSlide();
		StartNewPhysics(DeltaTime, Iterations);
		return;
	}

	// Surface gravity
	Velocity += Slide_GravityForce * FVector::DownVector * DeltaTime; // v += a * dt

	// Strafe
	if (FMath::Abs(FVector::DotProduct(Acceleration.GetSafeNormal(), UpdatedComponent->GetRightVector())) > .5)
	{
		Acceleration = Acceleration.ProjectOnTo(UpdatedComponent->GetRightVector()); // acceleration - world vector of WASD input
	}
	else
	{
		Acceleration = FVector::ZeroVector;
	}

	// Calculate velocity
	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		CalcVelocity(DeltaTime, Slide_Friction, true, GetMaxBrakingDeceleration());
	}
	ApplyRootMotionToVelocity(DeltaTime);

	// Perform Move
	Iterations++;
	bJustTeleported = false;

	// Store current position, location
	FVector OldLocation = UpdatedComponent->GetComponentLocation();
	FQuat OldRotation = UpdatedComponent->GetComponentRotation().Quaternion();
	// Hit result, movement delta
	FHitResult Hit(1.f);
	FVector Adjusted = Velocity * DeltaTime; // x = v * dt
	// Project velocity onto surface plane, create rotation matrix
	FVector VelPlaneDir = FVector::VectorPlaneProject(Velocity, SurfaceHit.Normal).GetSafeNormal();
	FQuat NewRotation = FRotationMatrix::MakeFromXZ(VelPlaneDir, SurfaceHit.Normal).ToQuat();
	// Move the component with desired rotation, collision checking (instead of changing capsule position)
	SafeMoveUpdatedComponent(Adjusted, NewRotation, true, Hit);

	// Check if hit something along the way
	if (Hit.Time < 1.f)
	{
		HandleImpact(Hit, DeltaTime, Adjusted);
		// Move the component velocity with the wall
		SlideAlongSurface(Adjusted, (1.f - Hit.Time), Hit.Normal, Hit, true);
	}

	// Exit slide if not on the surface, or current speed is less than min speed
	FHitResult NewSurfaceHit;
	if (!GetSlideSurface(NewSurfaceHit) || Velocity.SizeSquared() < pow(Slide_MinSpeed, 2))
	{
		ExitSlide();
	}

	// Stop velocity if character has stopped
	if (!bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / DeltaTime; // v = dx / dt
	}
}

bool UDemoCharacterMovementComponent::IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == InCustomMovementMode;
}
