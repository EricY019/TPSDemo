#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UBoxComponent;
class UProjectileMovementComponent;
class UParticleSystem;
class UParticleSystemComponent;
class USoundCue;
class AProjectileWeapon;

UCLASS()
class DEMO_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Owning Weapon
	AProjectileWeapon* OwningWeapon;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// On hit event, called on clients
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
	// Collision box
	UPROPERTY(EditAnywhere)
	UBoxComponent* CollisionBox;

	// Movement component
	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* ProjectileMovementComponent;

	// Tracer for projectile
	UPROPERTY(EditAnywhere)
	UParticleSystem* Tracer;
	UParticleSystemComponent* TracerComponent;
};
