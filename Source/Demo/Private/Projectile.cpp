#include "Projectile.h"
#include "ProjectileWeapon.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Demo/Demo.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	// Create projectile collision box
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);
	CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic); // moving objects
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); // enable physics and collision queries
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore); // init as ignore collision to all channels
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block); // line trace
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block); // static env object
	CollisionBox->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECollisionResponse::ECR_Block); // self-defined skeletal mesh
	// Create projectile movement component
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true; // projectile rotation following movement
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (Tracer)
	{	// spawn tracer component attached to projectile
		TracerComponent = UGameplayStatics::SpawnEmitterAttached(
			Tracer,
			CollisionBox,
			FName(),
			GetActorLocation(),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition);
	}
	// Bind on hit component on clients
	CollisionBox->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	FTransform Transform = GetActorTransform();
	FVector Location = GetActorLocation();
	if (OwningWeapon == nullptr)
	{	// double check on owner
		OwningWeapon = Cast<AProjectileWeapon>(GetOwner());
	}
	if (OwningWeapon)
	{
		OwningWeapon->OnHit(OtherActor, Transform, Location);
	}
	
	Destroy();
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

