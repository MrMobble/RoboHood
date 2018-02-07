// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboHood.h"
#include "RCharacter.h"

//All Includes For Other Scripts
#include "RWeapon.h"
#include "RPlayerController.h"
#include "RPlayerState.h"

//Other Includes
#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ARCharacter::ARCharacter()
{
 	//Set This Character To Call Tick() Every Frame.  You Can Turn This Off To Improve Performance If You Don't Need It.
	PrimaryActorTick.bCanEverTick = true;

	//Set Player Health
	Health = 100.f;

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
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

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
}

//This Function Is Part of the Multiplayer Framework And Replicated Variables Between Server And Clients
void ARCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARCharacter, Health);

	DOREPLIFETIME(ARCharacter, CurrentWeapon);

	DOREPLIFETIME(ARCharacter, Inventory);

	DOREPLIFETIME(ARCharacter, AimPitch);
}

//Initialise The Weapon
void ARCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

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
		// If the damage depletes our health set our lifespan to zero - which will destroy the actor  
		if (Health <= 0.f)
		{
			ARPlayerController* PlayerController = Cast<ARPlayerController>(Controller);
			if (PlayerController)
			{
				PlayerController->OnKilled();

				if (DamageCauser->GetInstigator() != this)
				{
					DamageCauser->GetInstigator()->PlayerState->Score += 1;
				}
			}

			DestroyInventory();
			Destroy();

		}
	}
	return ActualDamage;
}

//void ARCharacter::PawnClientRestart()
//{
//	Super::PawnClientRestart();
//
//	/* Equip the weapon on the client side. */
//	SetCurrentWeapon(CurrentWeapon);
//}

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
	if ((Controller != NULL) && (Value != 0.0f))
	{
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
		if (TheWeapon == CurrentWeapon) 
		{
			return;
		}

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
	PreviousWeapon = LastWeapon;

	ARWeapon* LocalLastWeapon = nullptr;
	if (LastWeapon) { LocalLastWeapon = LastWeapon; }

	if (LocalLastWeapon) { LocalLastWeapon->OnUnEquip(); }

	CurrentWeapon = NewWeapon;

	if (NewWeapon)
	{
		NewWeapon->SetOwningPawn(this);
		NewWeapon->OnEquip();
	}

	SwapToNewWeaponMesh();
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
	{
		CurrentWeapon->StartRecharge();
	}
}

void ARCharacter::AddAmmo(int32 AmmoIndex)
{
	if (CurrentWeapon)
	{
		Inventory[AmmoIndex]->IncreaseAmmo();
	}
}

// Called every frame
void ARCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//UE_LOG(LogTemp, Warning, TEXT("Your message"));

	SetPitch();
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