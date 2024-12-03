#include "ProjectileWeapon.h"
#include "DemoCharacter.h"
#include "Projectile.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

void AProjectileWeapon::BeginPlay()
{
	Super::BeginPlay();
	// Cooldown = 1 / fire rate
	if (RateOfFire > 0)
	{
		FireCooldown = 1.0f / RateOfFire;
	}
}

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	if (!bCanFire) return; // return if in cooldown

	// Set can fire boolean after cooldown
	bCanFire = false;
	GetWorld()->GetTimerManager().SetTimer(FireRateTimerHandle, this,
		&AProjectileWeapon::ResetFireCooldown, FireCooldown, false);
	
	APawn* InstigatorPawn = Cast<APawn>(GetOwner()); // cast owner to pawn
	const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName(FName("Muzzle"));
	if (MuzzleSocket)
	{
		// from muzzle socket to hit location from TraceUnderCrosshairs
		FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());
		FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		FRotator TargetRotation = ToTarget.Rotation();
		
		if (ProjectileClass && InstigatorPawn)
		{
			// Spawn parameters
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = InstigatorPawn;
			if (UWorld* World = GetWorld())
			{
				SpawnProjectile = World->SpawnActor<AProjectile>(ProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
				if (SpawnProjectile)
				{	// Set projectile properties
					SpawnProjectile->OwningWeapon = this;
				}
			}
		}
	}
}

void AProjectileWeapon::ResetFireCooldown()
{
	bCanFire = true;
}

void AProjectileWeapon::PlayFireOnhitAnim(const FTransform& Transform, const FVector& Location)
{
	if (ImpactParticles)
	{	// spawn impact particles at on hit location
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, Transform);
	}
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, Location);
	}
}

void AProjectileWeapon::OnHitEvent(AActor* OtherActor, float Damage, float OnHitTime,
	const FTransform& CurrentTransform, const FVector& CurrentLocation, const FVector& SpawnLocation)
{
	ServerOnHitEvent(OtherActor, Damage, OnHitTime, CurrentTransform, CurrentLocation, SpawnLocation);
}

void AProjectileWeapon::ServerOnHitEvent_Implementation(AActor* OtherActor, float Damage, float OnHitTime,
	const FTransform& CurrentTransform, const FVector& CurrentLocation, const FVector& SpawnLocation)
{
	// Determine if hit a character
	if (ADemoCharacter* HitCharacter = Cast<ADemoCharacter>(OtherActor))
	{
		// Validation - find hit actor past location
		FVector CharacterLocation = HitCharacter->GetPositionAtTime(OnHitTime);

		// Validation - determine the distance from actor location to bullet trace
		FVector ShootDirection = CurrentLocation - SpawnLocation;
		ShootDirection.Normalize();
		
		FVector ToCharacter = CharacterLocation - SpawnLocation;
		float ProjectionLength = FVector::DotProduct(ToCharacter, ShootDirection);

		FVector ProjectionPoint = SpawnLocation + ShootDirection * ProjectionLength;
		float Distance = FVector::Distance(CharacterLocation, ProjectionPoint);
		UE_LOG(LogTemp, Warning, TEXT("Distance: %f"), Distance);
		
		if (Distance > MarginDistance)
		{
			return;
		}
		
		// Apply damage authoritatively on server, damage replicated to clients with anim montage
		if (ACharacter* InstigatorCharacter = Cast<ACharacter>(this->GetOwner()))
		{
			if (AController* InstigatorController = InstigatorCharacter->GetController())
			{
				UGameplayStatics::ApplyDamage(OtherActor, Damage, InstigatorController, this, UDamageType::StaticClass());
			}
		}
	}
	

	// Multicast to other clients
	MulticastOnHitEvent(CurrentTransform, CurrentLocation);
}

void AProjectileWeapon::MulticastOnHitEvent_Implementation(const FTransform& CurrentTransform, const FVector& CurrentLocation)
{
	// Play projectile on hit animation
	PlayFireOnhitAnim(CurrentTransform, CurrentLocation);
}