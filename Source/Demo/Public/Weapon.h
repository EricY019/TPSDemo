#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class USphereComponent;
class UWidgetComponent;
class UAnimationAsset;
class UParticleSystem;
class UTexture2D;
class ADemoCharacter;

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	Ews_Initial UMETA(DisplayName = "Initial State"),
	Ews_Equipped UMETA(DisplayName = "Equipped"),
	Ews_Dropped UMETA(DisplayName = "Dropped"),
	Ews_Max UMETA(DisplayName = "DefaultMAX")
};

/**
 * 
 */
UCLASS()
class DEMO_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Setup Replication
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	// Show PickupWidget
	void ShowPickUpWidget(bool bShowWidget) const;
	// Set WeaponState
	void SetWeaponState(EWeaponState State);
	// Can fire boolean, for firing animation / cooldown calculation
	bool bCanFire = true;
	// Play firing animation, called on server
	void PlayFireAnim();
	// Firing function called on clients
	virtual void Fire(const FVector& HitTarget);
	// Drop weapon
	void Dropped();
	
	/**
	 * Textures for weapon crosshairs
	 */
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsCenter;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsLeft;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsRight;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsTop;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsBottom;
	
	// Zoomed FOV while aiming, modified in every weapon
	UPROPERTY(EditAnywhere)
	float ZoomedFOV = 50.f;

	UPROPERTY(EditAnywhere)
	float ZoomInterpSpeed = 30.f;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called on server when overlapping
	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	// Called on server when end overlapping
	UFUNCTION()
	void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);
	
private:
	// Weapon properties
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;
	
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USphereComponent* AreaSphere;
	
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UWidgetComponent* PickupWidget;

	// Called on client when WeaponState is replicated
	UFUNCTION()
	void OnRep_WeaponState();

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	UAnimationAsset* FireAnimation;

public:
	// WeaponState, replicated variable
	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState WeaponState;

	// Getters
	FORCEINLINE TObjectPtr<USphereComponent> GetAreaSphere() const {return AreaSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const {return WeaponMesh; }
	FORCEINLINE float GetZoomedFOV() const {return ZoomedFOV; }
	FORCEINLINE float GetZoomInterpSpeed() const {return ZoomInterpSpeed; }
};
