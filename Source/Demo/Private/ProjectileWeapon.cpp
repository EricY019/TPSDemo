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
		FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());
		// from muzzle socket to hit location from TraceUnderCrosshairs
		FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		FRotator TargetRotation = ToTarget.Rotation();
		if (ProjectileClass && InstigatorPawn)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this; // set owner of projectile to this weapon
			SpawnParams.Instigator = InstigatorPawn;
			if (UWorld* World = GetWorld())
			{	// spawn projectile in world
				Projectile = World->SpawnActor<AProjectile>(ProjectileClass,
					SocketTransform.GetLocation(), TargetRotation, SpawnParams);
				if (Projectile)
				{	// set owning weapon
					Projectile->OwningWeapon = this;
				}
			}
		}
	}
}

void AProjectileWeapon::ResetFireCooldown()
{
	bCanFire = true;
}

void AProjectileWeapon::PlayFireOnhitAnim(const FTransform& ProjectileTransform, const FVector& ProjectileLocation)
{
	if (ImpactParticles)
	{	// spawn impact particles at on hit location
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, ProjectileTransform);
	}
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, ProjectileLocation);
	}
}

void AProjectileWeapon::OnHit(AActor* OtherActor, FTransform ProjectileTransform, FVector ProjectileLocation)
{
	PlayFireOnhitAnim(ProjectileTransform, ProjectileLocation);
	ServerOnHit(OtherActor, ProjectileTransform, ProjectileLocation);
}

void AProjectileWeapon::ServerOnHit_Implementation(AActor* OtherActor, const FTransform& ProjectileTransform, const FVector& ProjectileLocation)
{
	MulticastOnHit(OtherActor, ProjectileTransform, ProjectileLocation);
}

void AProjectileWeapon::MulticastOnHit_Implementation(AActor* OtherActor, const FTransform& ProjectileTransform, const FVector& ProjectileLocation)
{
	ADemoCharacter* OwnerCharacter = Cast<ADemoCharacter>(GetOwner());
	if (OwnerCharacter && !(OwnerCharacter->IsLocallyControlled()))
	{
		PlayFireOnhitAnim(ProjectileTransform, ProjectileLocation);
	}
}