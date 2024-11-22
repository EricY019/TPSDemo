#include "DemoCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "Weapon.h"
#include "Net/UnrealNetwork.h"
#include "CharacterComponents/CombatComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "DemoAnimInstance.h"
#include "Demo/Demo.h"
#include "PlayerController/DemoPlayerController.h"

ADemoCharacter::ADemoCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// Disable character rotation when camera rotates
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationPitch = false;
	
	// Character movement config
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // rotation rate
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Mesh collision config, visibility block for line traces
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh); // set collision type as SkeletalMesh
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	
	// Create SpringArm for camera, attach to character mesh
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	
	// Create camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create Combat, replicated component
	Combat = CreateDefaultSubobject<UCombatComponent>("CombatComponent");

	// Turning in place
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	
	// Net update frequency
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.F;
}

void ADemoCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ADemoCharacter, OverlappingWeapon, COND_OwnerOnly); // Replicate OverlappingWeapon to owner proxies
	DOREPLIFETIME(ADemoCharacter, Health);
}

void ADemoCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();

	if (GetLocalRole() == ROLE_SimulatedProxy)
	{	// Call on movement replication
		SimProxiesTurn();
	}
	TimeSinceLastMovementReplication = 0.f;
}

void ADemoCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f); // Aiming offset bug fix, init value
	UpdateHUDHealth();
	// Receive damage on server
	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ADemoCharacter::ReceiveDamage);
	}
}

void ADemoCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (GetLocalRole() > ROLE_SimulatedProxy && IsLocallyControlled())
	{	// Obtain aiming offset for locally controlled player
		AimOffset(DeltaTime);
	}
	else
	{	// Replicate movement for simulated proxies
		TimeSinceLastMovementReplication += DeltaTime;
		if (TimeSinceLastMovementReplication > 0.25f)
		{
			OnRep_ReplicatedMovement();
		}
		CalculateAO_Pitch();
	}
}

void ADemoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMapping, 0);
		}
	}
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jump
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		// Move
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADemoCharacter::Move);
		// Look
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADemoCharacter::Look);
		// Equip Weapon
		EnhancedInputComponent->BindAction(EquipWeaponAction, ETriggerEvent::Triggered, this, &ADemoCharacter::EquipButtonPressed);
		// Aim
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &ADemoCharacter::AimButtonPressed);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ADemoCharacter::AimButtonReleased);
		// Fire
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &ADemoCharacter::FireButtonPressed);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ADemoCharacter::FireButtonReleased);
	}
}

void ADemoCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Combat)
	{
		Combat->Character = this;
	}
}

bool ADemoCharacter::IsWeaponEquipped()
{
	return (Combat && Combat->EquippedWeapon);
}

bool ADemoCharacter::IsAiming()
{
	return (Combat && Combat->bAiming);
}

AWeapon* ADemoCharacter::GetEquippedWeapon()
{
	if (Combat == nullptr) return nullptr;
	return Combat->EquippedWeapon;
}

void ADemoCharacter::PlayFireMontage(bool bAiming)
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName = bAiming ? FName("RifleAim") : FName("RifleHip"); // select section by if aiming
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ADemoCharacter::PlayHitReactMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		FName SectionName("FromFront");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ADemoCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MoveVector = Value.Get<FVector2D>();
	
	if (Controller)
	{
		// Find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		// Get forward, right vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(Rotation).GetUnitAxis(EAxis::Y);
		// Add movement
		AddMovementInput(ForwardDirection, MoveVector.X);
		AddMovementInput(RightDirection, MoveVector.Y);
	}
}

void ADemoCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookVector = Value.Get<FVector2D>();
	if (Controller)
	{
		AddControllerYawInput(LookVector.X);
		AddControllerPitchInput(LookVector.Y);
	}
}

void ADemoCharacter::ServerEquipButtonPressed_Implementation()
{	// Defines what happens on server
	if (Combat)
	{
		Combat->EquipWeapon(OverlappingWeapon);
	}
}

void ADemoCharacter::EquipButtonPressed()
{
	if (Combat)
	{
		if (HasAuthority()) // server-side
		{
			Combat->EquipWeapon(OverlappingWeapon);
		}
		else // client-side
		{
			ServerEquipButtonPressed();
		}
	}
}

void ADemoCharacter::AimButtonPressed()
{
	if (Combat)
	{
		Combat->SetAiming(true);
	}
}

void ADemoCharacter::AimButtonReleased()
{
	if (Combat)
	{
		Combat->SetAiming(false);
	}
}

void ADemoCharacter::FireButtonPressed()
{
	if (Combat)
	{
		Combat->FireButtonPressed(true);
	}
}

void ADemoCharacter::FireButtonReleased()
{
	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}
}

float ADemoCharacter::CalculateSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	return Velocity.Size();
}

void ADemoCharacter::AimOffset(float DeltaTime)
{
	if (Combat && Combat->EquippedWeapon == nullptr) return;
	
	float Speed = CalculateSpeed();
	bool bIsInAir = GetCharacterMovement()->IsFalling();
	
	// Set aiming offset yaw
	if (Speed == 0.f && !bIsInAir) // standing still
	{
		bRotateRootBone = true; // rotate root bone only when standing
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		if (TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			InterpAO_Yaw = AO_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime); // turn in place only when standing still
	}
	else if (Speed > 0.f || bIsInAir) // running, jumping
	{
		bRotateRootBone = false;
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning; // not turning if running / jumping
	}
	
	CalculateAO_Pitch();
}

void ADemoCharacter::CalculateAO_Pitch()
{
	AO_Pitch = GetBaseAimRotation().Pitch;
	// Pitch is bounded in [0, 360), [-90, 0) pitch is rounded to [270, 360) on server side
	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{	// 360.f - AO_Pitch does not work
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void ADemoCharacter::SimProxiesTurn()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	bRotateRootBone = false;
	float Speed = CalculateSpeed();
	if (Speed > 0.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}
	
	// Play turning animation when rotate > turning threshold
	ProxyRotationLastFrame = ProxyRotation;
	ProxyRotation = GetActorRotation();
	ProxyYaw = UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation, ProxyRotationLastFrame).Yaw;

	if (FMath::Abs(ProxyYaw) > TurnThreshold)
	{
		if (ProxyYaw > TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Right;
		}
		else if (ProxyYaw < -TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Left;
		}
	}
	else
	{
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}
}

void ADemoCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
}

void ADemoCharacter::TurnInPlace(float DeltaTime)
{
	if (AO_Yaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < -90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
	// reset interpAo_Yaw to 0 if turning
	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 4.f);
		AO_Yaw = InterpAO_Yaw;
		if (FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

void ADemoCharacter::OnRep_Health()
{
	// Called on clients, update HUD health, play hit react montage
	UpdateHUDHealth();
	PlayHitReactMontage();
}

void ADemoCharacter::UpdateHUDHealth()
{
	DemoPlayerController = DemoPlayerController == nullptr ? Cast<ADemoPlayerController>(Controller) : DemoPlayerController;
	if (DemoPlayerController)
	{
		DemoPlayerController->SetHUDHealth(Health, MaxHealth);
	}
}

void ADemoCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickUpWidget(false);
	}
	OverlappingWeapon = Weapon;
	if (IsLocallyControlled()) // true if this character is controlled by local player (server player)
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickUpWidget(true);
		}
	}
}

void ADemoCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{	// LastWeapon be the last value before replication happens
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickUpWidget(true);
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickUpWidget(false);
	}
}
