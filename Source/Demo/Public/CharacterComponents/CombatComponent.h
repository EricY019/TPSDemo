#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HUD/DemoHUD.h"
#include "CombatComponent.generated.h"

#define TRACE_LENGTH 80000
class ADemoCharacter;
class AWeapon;
class APlayerController;
class ADemoHUD;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEMO_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponent();
	// DemoCharacter can access members of CombatComponent
	friend class ADemoCharacter;
	// Set replicates
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	// Equip weapon
	void EquipWeapon(AWeapon* WeaponToEquip);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	// Set aiming
	void SetAiming(bool bIsAiming);
	// RPC, server sets aiming, called by client
	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);
	// Called on clients when EquippedWeapon is replicated
	UFUNCTION()
	void OnRep_EquippedWeapon();
	// Called when fire button pressed
	void FireButtonPressed(bool bPressed);
	/*
	 * Play firing animation, RPC and multicast
	 */
	UFUNCTION(Server, Reliable)
	void ServerFire();
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire();
	// Build trace given a hit result
	void TraceUnderCrosshairs(FHitResult& TraceHitResult);
	// Set HUD crosshairs
	void SetHUDCrosshairs(float DeltaType);

private:
	ADemoCharacter* Character;
	APlayerController* Controller;
	ADemoHUD* HUD;
	
	bool bFireButtonPressed;
	/**
	 * HUD and Crosshairs
	 */
	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootFactor;
	
	FVector HitTarget;
	FHUDPackage HUDPackage;

	/*
	 * Aiming and FOV
	 */
	// Field of view when not aiming, set to the camera's base FOV in BeginPlay
	float DefaultFOV;
	float CurrentFOV;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomedFOV = 30.f;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomInterpSpeed = 20.f;

	void InterpFOV(float DeltaTime);

public:
	// Equipped weapon, replicated variable
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeapon* EquippedWeapon;
	
	// isAiming, replicated variable
	UPROPERTY(Replicated)
	bool bAiming;
};
