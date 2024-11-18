#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"


#define TRACE_LENGTH 80000
class ADemoCharacter;
class AWeapon;

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
	// RPC, server fires, called by client
	UFUNCTION(Server, Reliable)
	void ServerFire();
	// Multicast to all clients, called by server
	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire();
	// Build trace given a hit result
	void TraceUnderCrosshairs(FHitResult& TraceHitResult);

private:
	TObjectPtr<ADemoCharacter> Character;
	bool bFireButtonPressed;
	// Hit target set every tick
	FVector HitTarget;

public:
	// Equipped weapon, replicated variable
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	TObjectPtr<AWeapon> EquippedWeapon;
	
	// isAiming, replicated variable
	UPROPERTY(Replicated)
	bool bAiming;
};
