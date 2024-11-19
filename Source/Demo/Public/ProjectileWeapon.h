#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "ProjectileWeapon.generated.h"

class AProjectile;

UCLASS()
class DEMO_API AProjectileWeapon : public AWeapon
{
	GENERATED_BODY()
	
public:
	virtual void Fire(const FVector& HitTarget) override;
	
private:
	// projectile class
	UPROPERTY(EditAnywhere)
	TSubclassOf<AProjectile> ProjectileClass;
};
