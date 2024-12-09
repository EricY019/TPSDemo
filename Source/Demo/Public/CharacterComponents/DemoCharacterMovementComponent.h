#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DemoCharacterMovementComponent.generated.h"

class ADemoCharacter;

UENUM(BlueprintType)
enum ECustomMovementMode
{
	CMOVE_None	UMETA(Hidden),
	CMOVE_Slide	UMETA(DisplayName = "Slide"),
	CMOVE_Max	UMETA(Hidden),
};

/**
 * 
 */
UCLASS()
class DEMO_API UDemoCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	// Snapshot of all data required to produce a move in a single frame, replicating to server
	class FSavedMove_Demo : public FSavedMove_Character
	{
		typedef FSavedMove_Character Super;
	public:
		uint8 Saved_bWantsToSprint:1;
		uint8 Saved_bWantsToSlide:1;
		
		FSavedMove_Demo();
		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
		virtual void Clear() override;
		virtual uint8 GetCompressedFlags() const override;
		virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData) override;
		virtual void PrepMoveFor(ACharacter* C) override;
	};

	// Client prediction, using FSavedMove_Demo
	class FNetworkPredictionData_Client_Demo : public FNetworkPredictionData_Client_Character
	{
	public:
		FNetworkPredictionData_Client_Demo(const UCharacterMovementComponent& ClientMovement);

		typedef FNetworkPredictionData_Client_Character Super;

		virtual FSavedMovePtr AllocateNewMove() override;
	};
	
public:
	// Constructor
	UDemoCharacterMovementComponent();

	// Client prediction
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

	// Sprint non-safe functions, only callable on clients
	UFUNCTION(BlueprintCallable)
	void SprintPressed();
	UFUNCTION(BlueprintCallable)
	void SprintReleased();
	// Sliding non-safe functions, only callable on clients
	UFUNCTION(BlueprintCallable)
	void SlidePressed();
	
	// General custom movement mode feature
	UFUNCTION(BlueprintPure)
	bool IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const;

	// Sprint params
	UPROPERTY(EditDefaultsOnly, Category = "Sprint")
	float Sprint_MaxWalkSpeed;
	UPROPERTY(EditDefaultsOnly, Category = "Sprint")
	float Walk_MaxWalkSpeed;
	
	// Slide Params
	UPROPERTY(EditDefaultsOnly, Category = "Slide")
	float Slide_MinSpeed = 350;
	UPROPERTY(EditDefaultsOnly, Category = "Slide")
	float Slide_EnterImpulse = 500;
	UPROPERTY(EditDefaultsOnly, Category = "Slide")
	float Slide_GravityForce = 5000; // keep the player to the ground, on the slope
	UPROPERTY(EditDefaultsOnly, Category = "Slide")
	float Slide_Friction = 1.3;
	UPROPERTY(EditDefaultsOnly, Category = "Slide")
	float DefaultCapsuleHalfHeight = 88.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Slide")
	float SlideCapsuleHalfHeight = 44.0f;
	
	// Transient
	UPROPERTY(Transient) ADemoCharacter* DemoCharacterOwner;

	// Safe booleans
	UPROPERTY(BlueprintReadWrite)
	bool Safe_bWantsToSprint;
	UPROPERTY(BlueprintReadWrite, Replicated)
	bool Safe_bWantsToSlide;
	
protected:
	// Init Component
	virtual void InitializeComponent() override;

	// Replication
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	
	// Called at the end of every perform move
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;

	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;

	// Physics that handles custom input movement mode
	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;

	
private:
	// Sliding functions
	void EnterSlide();
	void ExitSlide();
	bool GetSlideSurface(FHitResult& Hit) const; // return if is on a sliding surface
	void PhysSlide(float DeltaTime, int32 Iterations);
};
