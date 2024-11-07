#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class AWeapon;
class ADemoCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEMO_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponent();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	// DemoCharacter can access members of CombatComponent
	friend class ADemoCharacter;
	// Equip weapon
	void EquipWeapon(AWeapon* WeaponToEquip);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	TObjectPtr<AWeapon> EquippedWeapon;
	TObjectPtr<ADemoCharacter> Character;
};
