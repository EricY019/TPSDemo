// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DemoCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class AWeapon;
class UCombatComponent;
struct FInputActionValue;

UCLASS()
class DEMO_API ADemoCharacter : public ACharacter
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
	bool IsAiming();\
	// AO_Yaw getter
	FORCEINLINE float GetAOYaw() const {return AO_Yaw; }
	// AO_Pitch getter
	FORCEINLINE float GetAOPitch() const {return AO_Pitch; }
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Handle 2d move, 2d camera turn, equip weapon, aim
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void EquipButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	// Obtain aiming offset, called per frame
	void AimOffset(float DeltaTime);

private:
	// SpringArm for Camera
	UPROPERTY(VisibleAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"));
	TObjectPtr<USpringArmComponent> CameraBoom;
	
	// Camera
	UPROPERTY(VisibleAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;
	
	// Input mapping context
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMapping;

	// Enhanced input components: Jump, move, look, equip, aim
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> JumpAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> EquipWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> AimAction;
	
	// Called on client when OverlappingWeapon is replicated
	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);
	
	// Combat Component
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCombatComponent> Combat;
	
	// RPC, clients call for server to execute
	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();
	
	// for aim offset calculation
	float AO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;
	
public:
	// OverlappingWeapon, replicated variable
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	TObjectPtr<AWeapon> OverlappingWeapon;
};
