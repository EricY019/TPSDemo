#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

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
	// RPC, clients call on server to execute
	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);
	// Called on clients when EquippedWeapon is replicated
	UFUNCTION()
	void OnRep_EquippedWeapon();

private:
	TObjectPtr<ADemoCharacter> Character;

public:
	// Equipped weapon, replicated variable
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	TObjectPtr<AWeapon> EquippedWeapon;
	
	// isAiming, replicated variable
	UPROPERTY(Replicated)
	bool bAiming;
};
