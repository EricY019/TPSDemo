#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "ProjectileWeapon.generated.h"

class AProjectile;

/**
 * 
 */
UCLASS()
class DEMO_API AProjectileWeapon : public AWeapon
{
	GENERATED_BODY()
	
public:
	virtual void Fire(const FVector& HitTarget) override;
	void OnHitEvent(AActor* OtherActor, AWeapon* CausingWeapon, const float& Damage, const FTransform& ProjectileTransform, const FVector& ProjectileLocation);

protected:
	// BeginPlay override
	virtual void BeginPlay() override;
	/*
	 *	Play on hit animation, RPC and multicast
	 */
	UFUNCTION(Server, Reliable)
	void ServerOnHitEvent(AActor* OtherActor, AWeapon* CausingWeapon, const float& Damage, const FTransform& ProjectileTransform, const FVector& ProjectileLocation);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnHitEvent(const FTransform& ProjectileTransform, const FVector& ProjectileLocation);
	
private:
	// Projectile subclass, spawn from projectile weapon
	UPROPERTY(EditAnywhere)
	TSubclassOf<AProjectile> ProjectileClass;

	// Projectile pointer
	UPROPERTY()
	AProjectile* Projectile;

	// Tracer, sound for projectile hit impacts
	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticles;
	UPROPERTY(EditAnywhere)
	USoundCue* ImpactSound;

	// Cooldown
	UPROPERTY(EditAnywhere, Category = "Weapon")
	float RateOfFire = 3.0f;
	float FireCooldown = 0.0f;
	FTimerHandle FireRateTimerHandle;
	// reset cooldown
	void ResetFireCooldown();
	
	// Play firing on hit anim
	void PlayFireOnhitAnim(const FTransform& ProjectileTransform, const FVector& ProjectileLocation);
};
