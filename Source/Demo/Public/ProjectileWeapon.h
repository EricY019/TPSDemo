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
	AProjectileWeapon();
	virtual void Fire(const FVector& HitTarget) override;
	virtual void OnHit(AActor* OtherActor, FTransform ProjectileTransform, FVector ProjectileLocation) override;

protected:
	// RPC, execute on server, called on clients
	UFUNCTION(Server, Reliable)
	void ServerOnHit(AActor* OtherActor, FTransform ProjectileTransform, FVector ProjectileLocation);
	// Multicast to all clients
	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnHit(AActor* OtherActor, FTransform ProjectileTransform, FVector ProjectileLocation);
	
private:
	// projectile subclass, spawn from projectile weapon
	UPROPERTY(EditAnywhere)
	TSubclassOf<AProjectile> ProjectileClass;

	// projectile pointer
	UPROPERTY()
	TObjectPtr<AProjectile> Projectile;

	// Tracer, sound for projectile hit impacts
	UPROPERTY(EditAnywhere)
	TObjectPtr<UParticleSystem> ImpactParticles;
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundCue> ImpactSound;
};
