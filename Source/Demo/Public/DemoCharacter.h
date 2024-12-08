#pragma once

#include "CoreMinimal.h"
#include "CharacterComponents/DemoCharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Demo/Public/CharacterTypes/TurningInPlace.h"
#include "Demo/Public/Interfaces/InteractWithCrosshairsInterface.h"
#include "DemoCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class AWeapon;
class UCombatComponent;
class UAnimMontage;
class ADemoPlayerController;
class AController;
class UDemoCharacterMovementComponent;
struct FInputActionValue;

USTRUCT()
struct FPositionHistoryEntry
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Position;

	UPROPERTY()
	float Time;

	FPositionHistoryEntry() : Position(FVector::ZeroVector), Time(0.0f) {}
	FPositionHistoryEntry(const FVector& InPosition, float InTime) : Position(InPosition), Time(InTime) {}
};

/**
 * 
 */
UCLASS()
class DEMO_API ADemoCharacter : public ACharacter, public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADemoCharacter(const FObjectInitializer& ObjectInitializer);
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// Set Replicates
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	// Called on server, set OverlappingWeapon as weapon
	void SetOverlappingWeapon(AWeapon* Weapon);
	// Determines if this character equips weapon
	bool IsWeaponEquipped();
	// Determines if this character is aiming
	bool IsAiming();
	// Get EquippedWeapon
	AWeapon* GetEquippedWeapon();
	// Play fire montage if aiming
	void PlayFireMontage(bool bAiming);
	// Play elimation montage
	void PlayElimMontage();
	// Play on hit montage
	void PlayHitReactMontage();
	// Override OnRep_ReplicatedMovement
	virtual void OnRep_ReplicatedMovement() override;
	// On server elim
	void Elim();
	// Multicast elim, when player is eliminated
	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim();
	// RPC, update position on server
	UFUNCTION(Server, Reliable)
	void ServerUpdatePosition(const FVector& NewPosition, float TimeStamp);
	// Retrieves the position of the player at a specific time, called on server
	FVector GetPositionAtTime(float ServerTime) const;
	// Return collision ignore params
	FCollisionQueryParams GetIgnoreCharacterParams() const;

protected:
	// Character Movement Component
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	UDemoCharacterMovementComponent* DemoCharacterMovementComponent;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Handle 2d move, 2d camera turn, equip weapon, aim, fire, sprint
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void EquipButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	void FireButtonPressed();
	void FireButtonReleased();
	void SprintButtonPressed();
	void SprintButtonReleased();
	// Obtain aiming offset, called per frame
	void AimOffset(float DeltaTime);
	// Calculate AO_Pitch
	void CalculateAO_Pitch();
	// Handle turning for simulated proxies
	void SimProxiesTurn();
	// Callback to damage event
	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser);
	// Update HUD health
	void UpdateHUDHealth();
	
private:
	// SpringArm for Camera
	UPROPERTY(VisibleAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"));
	USpringArmComponent* CameraBoom;
	
	// Camera
	UPROPERTY(VisibleAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	// Input mapping context
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMapping;

	// Enhanced input components: Jump, move, look, equip, aim, fire, slide
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* EquipWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* SprintAction;
	
	// Called on client when OverlappingWeapon is replicated
	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);
	
	// Combat Component
	UPROPERTY(VisibleAnywhere)
	UCombatComponent* Combat;
	
	// RPC, clients call for server to execute
	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();
	
	// Aim offset calculation
	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	// Turning in place
	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);
	
	// Animation montages
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* FireWeaponMontage;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* ElimMontage;

	// Rotation for simulated proxy
	bool bRotateRootBone;
	float TurnThreshold = 0.5f; // threshold
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TimeSinceLastMovementReplication;
	float CalculateSpeed();
	
	// Player health
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.f;
	ADemoPlayerController* DemoPlayerController;
	
	UFUNCTION()
	void OnRep_Health();
	
	// Eliminated functions 
	bool bElimmed = false;
	FTimerHandle ElimTimer;
	
	UPROPERTY(EditDefaultsOnly)
	float ElimDelay = 0.9f;

	void ElimTimerFinished();
	
	// Max history location duration in seconds, default 1.5s
	UPROPERTY(EditDefaultsOnly, Category = "Position History")
	float MaxHistoryDuration = 1.5f;

public:
	// OverlappingWeapon, replicated variable
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	AWeapon* OverlappingWeapon;

	// Player health, replicated variable
	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
	float Health = 100.f;

	// Circular list to store past positions with timestamp index, server-side update, replicated variable
	UPROPERTY(Replicated)
	TArray<FPositionHistoryEntry> PositionHistory;

	// Getters
	FORCEINLINE float GetAOYaw() const {return AO_Yaw; }
	FORCEINLINE float GetAOPitch() const {return AO_Pitch; }
	FORCEINLINE ETurningInPlace GetTurningInPlace() const {return TurningInPlace; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const {return FollowCamera; }
	FORCEINLINE bool ShouldRoatateRootBone() const {return bRotateRootBone; }
	FORCEINLINE bool IsElimmed() const {return bElimmed; }
};