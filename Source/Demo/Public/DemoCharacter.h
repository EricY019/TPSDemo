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
	UPROPERTY(VisibleAnywhere, Replicated)
	bool IsWeaponEquipped;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Handle 2d move, 2d camera turn, equip weapon
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void EquipButtonPressed();

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

	// Enhanced input components: Jump, move, look
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> JumpAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> EquipWeaponAction;

	// Replicate variable from server to client
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	TObjectPtr<AWeapon> OverlappingWeapon;
	
	// Called on client when OverlappingWeapon is replicated
	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	// Combat Component
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCombatComponent> Combat;
	
	// RPC, Called on clients to execute on server
	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();
};
