#include "ProjectileWeapon.h"

#include "DemoCharacter.h"
#include "Projectile.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

AProjectileWeapon::AProjectileWeapon()
{
	bReplicates = true;
}

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
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
			SpawnParams.Owner = GetOwner(); // set owner of projectile to owner of weapon
			SpawnParams.Instigator = InstigatorPawn;
			if (UWorld* World = GetWorld())
			{	// spawn projectile in world
				Projectile = World->SpawnActor<AProjectile>(ProjectileClass,
					SocketTransform.GetLocation(), TargetRotation, SpawnParams);
				Projectile->OwningWeapon = this;
			}
		}
	}
}

void AProjectileWeapon::OnHit(AActor* OtherActor, FTransform ProjectileTransform, FVector ProjectileLocation)
{
	// Play on hit anim and sound
	if (ImpactParticles)
	{	// spawn impact particles at on hit location
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, ProjectileTransform);
	}
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, ProjectileLocation);
	}
	ServerOnHit_Implementation(OtherActor, ProjectileTransform, ProjectileLocation);
}


void AProjectileWeapon::ServerOnHit_Implementation(AActor* OtherActor, FTransform ProjectileTransform, FVector ProjectileLocation)
{
	MulticastOnHit(OtherActor, ProjectileTransform, ProjectileLocation);
}

void AProjectileWeapon::MulticastOnHit_Implementation(AActor* OtherActor, FTransform ProjectileTransform,
	FVector ProjectileLocation)
{
	ADemoCharacter* OwnerCharacter = Cast<ADemoCharacter>(GetOwner());
	if (OwnerCharacter)
	{
		// Get the local player controller
		APlayerController* LocalPC = GetWorld()->GetFirstPlayerController();
		
		// Check if this character is controlled by the local player controller
		if (OwnerCharacter->GetController() == LocalPC)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Local player hit"));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Remote player hit"));
		}
	}
	
	
	// if (ImpactParticles)
	// {	// spawn impact particles at on hit location
	// 	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, ProjectileTransform);
	// }
	// if (ImpactSound)
	// {
	// 	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, ProjectileLocation);
	// }
}