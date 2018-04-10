// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboHood.h"
#include "RCharacter.h"

//All Includes For Other Scripts
#include "RWeapon.h"
#include "RPlayerController.h"
#include "RPlayerState.h"
#include "RGameMode.h"

//Other Includes
#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ARCharacter::ARCharacter()
{
 	//Set This Character To Call Tick() Every Frame.  You Can Turn This Off To Improve Performance If You Don't Need It.
	PrimaryActorTick.bCanEverTick = true;

	CurrentWeaponIndex = 0;

	//Set Size For Collision Capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	//Set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	//Don't Rotate When The Controller Rotates. Let That Just Affect The Camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	//Configure Character Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	//Create A Camera Boom (Pulls In Towards The Player If There Is A Collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;	
	CameraBoom->bUsePawnControlRotation = true;

	//Create a follow camera
	TPPCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TPPCamera"));
	TPPCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TPPCamera->bUsePawnControlRotation = false;

	//FPPCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FPPCamera"));
	//FPPCamera->SetupAttachment(GetMesh());

	//Enabled Variable Replication MAYBE! (Might be outdated and not needed)
	bReplicates = true;
}

//Called To Bind Functionality To Input
void ARCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Set Up Gameplay Key Bindings
	check(PlayerInputComponent);

	//Jump Input
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	//Movement Input
	PlayerInputComponent->BindAxis("MoveForward", this, &ARCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ARCharacter::MoveRight);

	//Camera Input
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ARCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ARCharacter::LookUpAtRate);

	//FireWeapon Input
	PlayerInputComponent->BindAction("LeftMouse", IE_Pressed, this, &ARCharacter::OnStartFire);
	PlayerInputComponent->BindAction("LeftMouse", IE_Released, this, &ARCharacter::OnStopFire);

	//Reload Input
	PlayerInputComponent->BindAction("Reload", IE_Released, this, &ARCharacter::ReloadWeapon);

	//ChangeWeapon Input
	PlayerInputComponent->BindAction("WeaponOne", IE_Released, this, &ARCharacter::OneAction);
	PlayerInputComponent->BindAction("WeaponTwo", IE_Released, this, &ARCharacter::TwoAction);
	PlayerInputComponent->BindAction("WeaponThree", IE_Released, this, &ARCharacter::ThreeAction);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ARCharacter::StartRunning);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ARCharacter::StopRunning);
}

//This Function Is Part of the Multiplayer Framework And Replicated Variables Between Server And Clients
void ARCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARCharacter, Health);

	DOREPLIFETIME(ARCharacter, CurrentWeapon);

	DOREPLIFETIME(ARCharacter, Inventory);

	DOREPLIFETIME(ARCharacter, AimPitch);

	DOREPLIFETIME(ARCharacter, LastTakeHitInfo);
}

//Initialise The Weapon
void ARCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Health = Base_Health;

	GetCharacterMovement()->MaxWalkSpeed = Walk_Speed;

	SpawnWeapons();
}

void ARCharacter::SpawnWeapons()
{
	if (Role == ROLE_Authority)
	{
		for (int32 i = 0; i < Weapons.Num(); i++)
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			ARWeapon* NewWeapon = GetWorld()->SpawnActor<ARWeapon>(Weapons[i], SpawnInfo);

			AddWeapon(NewWeapon);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Other Character Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// This is the actor damage handler.   
float ARCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	// Call the base class - this will tell us how much damage to apply  
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if (ActualDamage > 0.f)
	{
		Health -= ActualDamage;
		if (Health <= 0.f)
		{
			if (EventInstigator && DamageCauser)
			{
				Die(EventInstigator, DamageCauser);
			}
		}
	}
	return ActualDamage;
}

//Tells gamemode that you died.
bool ARCharacter::Die(class AController* Killer, class AActor* DamageCauser)
{
	if (!CanDie()) return false;
	if (Killer == NULL || DamageCauser == NULL) return false;

	Health = FMath::Min(0.0f, Health);
	AController* const KilledPlayer = (Controller != NULL) ? Controller : Cast<AController>(GetOwner());

	//Tell the gamemode that someone died (gamemode handles adding score and respawning player).
	GetWorld()->GetAuthGameMode<ARGameMode>()->Killed(Killer, KilledPlayer, this);

	GetCharacterMovement()->ForceReplicationUpdate();
	
	OnDeath(Killer, DamageCauser);
	return true;
}

//Handles death
void ARCharacter::OnDeath(class AController* Killer, class AActor* DamageCauser)
{
	if (bIsDying) return;

	bReplicateMovement = false;
	bIsDying = true;

	if (Role == ROLE_Authority)
	{
		ReplicateHit(Killer, DamageCauser, true);
	}

	DestroyInventory();

	DetachFromControllerPendingDestroy();

	if (GetMesh())
	{
		static FName CollisionProfileName(TEXT("Ragdoll"));
		GetMesh()->SetCollisionProfileName(CollisionProfileName);
	}
	SetActorEnableCollision(true);

	SetRagDoll();

	// disable collisions on capsule
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
}

//Apply ragdoll physics.
void ARCharacter::SetRagDoll()
{
	bool bInRagdoll = false;

	if (IsPendingKill())
	{
		bInRagdoll = false;
	}
	else if (!GetMesh() || !GetMesh()->GetPhysicsAsset())
	{
		bInRagdoll = false;
	}
	else
	{
		// initialize physics/etc
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->WakeAllRigidBodies();
		GetMesh()->bBlendPhysics = true;

		bInRagdoll = true;
	}

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->SetComponentTickEnabled(false);

	if (!bInRagdoll)
	{
		// hide and set short lifespan
		TurnOff();
		SetActorHiddenInGame(true);
		SetLifeSpan(1.0f);
	}
	else
	{
		SetLifeSpan(10.0f);
	}
}

//Replicates the hit that killed the player.
void ARCharacter::ReplicateHit(class AController* Killer, class AActor* DamageCauser, bool bKilled)
{
	LastTakeHitInfo.Killer = Killer;
	LastTakeHitInfo.DamageCauser = DamageCauser;
	LastTakeHitInfo.bKilled = bKilled;
}

//On Replication function
void ARCharacter::OnRep_LastTakeHitInfo()
{
	if (LastTakeHitInfo.bKilled)
	{
		OnDeath(LastTakeHitInfo.Killer, LastTakeHitInfo.DamageCauser);
	}
}

//Check if player can die
bool ARCharacter::CanDie()
{
	if (bIsDying || IsPendingKill() || Role != ROLE_Authority || GetWorld()->GetAuthGameMode<ARGameMode>() == NULL)
	{
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Character Movement Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Camera Left/Right
void ARCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

//Camera Up/Down
void ARCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

//Move Forward/Back Function
void ARCharacter::MoveForward(float Value)
{
	fDirection = Value;

	if ((Controller != NULL) && (Value != 0.0f))
	{
		if (Value < 0) SetRunning(false);

		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		AddMovementInput(Direction, Value);
	}
}

//Move Left/Right Function
void ARCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		if (bWantsToRun) SetRunning(false);

		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(Direction, Value);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// All Weapon Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ARCharacter::AddWeapon(ARWeapon* NewWeapon)
{
	if (NewWeapon && Role == ROLE_Authority)
	{
		NewWeapon->OnEnterInventory(this);
		Inventory.Add(NewWeapon);

		// Equip first weapon in inventory
		if (Inventory.Num() > 2 && CurrentWeapon == nullptr)
		{
			EquipWeapon(Inventory[0]);
		}
	}
}

void ARCharacter::EquipWeapon(ARWeapon* TheWeapon)
{
	//If The Weapon Is Valid
	if (TheWeapon)
	{
		if (TheWeapon == CurrentWeapon) return;
		
		if (Role == ROLE_Authority) { SetCurrentWeapon(TheWeapon, CurrentWeapon); }
		else { ServerEquipWeapon(TheWeapon); }
	}
}

// Server Functions
void ARCharacter::ServerEquipWeapon_Implementation(ARWeapon* TheWeapon) { EquipWeapon(TheWeapon); }
bool ARCharacter::ServerEquipWeapon_Validate(ARWeapon* TheWeapon) { return true; }

//OnRep Function
void ARCharacter::OnRep_CurrentWeapon(ARWeapon* LastWeapon)
{ 
	SetCurrentWeapon(CurrentWeapon, LastWeapon);
}

void ARCharacter::SetCurrentWeapon(class ARWeapon* NewWeapon, class ARWeapon* LastWeapon)
{
	ARWeapon* LocalLastWeapon = nullptr;

	if (LastWeapon != NULL)
	{
		LocalLastWeapon = LastWeapon;
	}
	else if (NewWeapon != CurrentWeapon)
	{
		LocalLastWeapon = CurrentWeapon;
	}

	//Unequip Previous Weapon
	if (LocalLastWeapon)
	{
		LocalLastWeapon->OnUnEquip();
	}

	CurrentWeapon = NewWeapon;

	//Equip New Weapon
	if (NewWeapon)
	{
		NewWeapon->SetOwningPawn(this);

		NewWeapon->OnEquip();
	}

	//SwapToNewWeaponMesh(); New Method Does Not Use This
}

FName ARCharacter::GetWeaponAttachPoint() { return WeaponAttachPoint; }

void ARCharacter::OneAction()
{
	CurrentWeaponIndex = 0;
	EquipWeapon(Inventory[0]);
}

void ARCharacter::TwoAction()
{
	CurrentWeaponIndex = 1;
	EquipWeapon(Inventory[1]);
}

void ARCharacter::ThreeAction()
{
	CurrentWeaponIndex = 2;
	EquipWeapon(Inventory[2]);
}

void ARCharacter::SwapToNewWeaponMesh()
{
	if (PreviousWeapon)
	{
		PreviousWeapon->AttachMeshToPawn("Storage");
	}

	if (CurrentWeapon)
	{
		CurrentWeapon->AttachMeshToPawn("Hand");
	}
}

void ARCharacter::DestroyInventory()
{
	for (int32 I = 0; I < Inventory.Num(); I++)
	{
		ARWeapon* Weapon = Inventory[I];
		if (Weapon)
		{
			Weapon->Destroy();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ShootWeapon Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ARCharacter::OnStartFire() { StartWeaponFire(); }

void ARCharacter::OnStopFire() { StopWeaponFire(); }

void ARCharacter::StartWeaponFire()
{
	if (bWantsToRun) return;

	if (!bWantsToFire)
	{
		bWantsToFire = true;
		if (CurrentWeapon) { CurrentWeapon->StartFire(); }
	}
}

void ARCharacter::StopWeaponFire()
{
	if (bWantsToFire)
	{
		bWantsToFire = false;
		if (CurrentWeapon) { CurrentWeapon->StopFire(); }
	}
}

void ARCharacter::ReloadWeapon()
{
	if (CurrentWeapon)
		CurrentWeapon->StartRecharge();
}

void ARCharacter::AddAmmo(int32 AmmoIndex, int32 Ammount)
{
	if (CurrentWeapon)
		Inventory[AmmoIndex]->IncreaseAmmo(Ammount);
}

void ARCharacter::AddHealth(int32 Ammount)
{
	Health += Ammount;

	//Checks if health is greater then base health.
	if (Health > Base_Health) Health = Base_Health;
}

// Called every frame
void ARCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	SetPitch();

	if (bShiftPressed)
		if (fDirection > 0) SetRunning(true);
}

bool ARCharacter::IsAlive()
{
	return Health > 0;
}

void ARCharacter::SetPitch()
{
	if (Role == ROLE_Authority)
	{
		FRotator Delta = GetControlRotation() - GetActorRotation();
		Delta.Normalize();

		AimPitch = Delta.Pitch;
	}
}

bool ARCharacter::IsRunning()
{
	return (bWantsToRun) && !GetVelocity().IsZero() && (GetVelocity().GetSafeNormal2D() | GetActorForwardVector()) > -0.1;
}

void ARCharacter::StartRunning()
{
	bShiftPressed = true;
}

void ARCharacter::StopRunning()
{
	bShiftPressed = false;
	SetRunning(false);
}

void ARCharacter::SetRunning(bool bNewValue)
{
	bWantsToRun = bNewValue;

	if (bNewValue) GetCharacterMovement()->MaxWalkSpeed = Run_Speed;	
	else if (!bNewValue) GetCharacterMovement()->MaxWalkSpeed = Walk_Speed;
	
	if (bWantsToRun) StopWeaponFire();

	if (Role < ROLE_Authority)
	{
		ServerSetSprint(bNewValue);
	}
}

void ARCharacter::ServerSetSprint_Implementation(bool bNewValue)
{
	SetRunning(bNewValue);
}
