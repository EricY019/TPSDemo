// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
struct FInputActionValue;

UCLASS()
class DEMO_API ADemoCharacter : public ACharacter, public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADemoCharacter();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// Set Replicates
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	// Called on server, set OverlappingWeapon as weapon
	void SetOverlappingWeapon(AWeapon* Weapon);
	// Init component reference
	virtual void PostInitializeComponents() override;
	// Determines if this character equips weapon
	bool IsWeaponEquipped();
	// Determines if this character is aiming
	bool IsAiming();
	// Get EquippedWeapon
	AWeapon* GetEquippedWeapon();
	// Play fire montage if aiming
	void PlayFireMontage(bool bAiming);
	// Play on hit montage
	void PlayHitReactMontage();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Handle 2d move, 2d camera turn, equip weapon, aim, fire
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void EquipButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	void FireButtonPressed();
	void FireButtonReleased();
	// Obtain aiming offset, called per frame
	void AimOffset(float DeltaTime);

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

	// Enhanced input components: Jump, move, look, equip, aim
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

	// Anim montage for firing weapon
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* FireWeaponMontage;

	// Anim montage for hit react
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* HitReactMontage;
	
	
public:
	// OverlappingWeapon, replicated variable
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	AWeapon* OverlappingWeapon;

	// Getters
	FORCEINLINE float GetAOYaw() const {return AO_Yaw; }
	FORCEINLINE float GetAOPitch() const {return AO_Pitch; }
	FORCEINLINE ETurningInPlace GetTurningInPlace() const {return TurningInPlace; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const {return FollowCamera; }
};
