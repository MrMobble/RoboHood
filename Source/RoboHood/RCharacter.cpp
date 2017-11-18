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

// Sets default values
ARCharacter::ARCharacter()
{
 	//Set This Character To Call Tick() Every Frame.  You Can Turn This Off To Improve Performance If You Don't Need It.
	PrimaryActorTick.bCanEverTick = false;

	//Set Player Health
	Health = 100.f;

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
}

//This Function Is Part of the Multiplayer Framework And Replicated Variables Between Server And Clients
void ARCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARCharacter, Health);

	DOREPLIFETIME(ARCharacter, CurrentWeapon);
}

//Initialise The Weapon
void ARCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Role == ROLE_Authority) 
	{ 
		if (DefaultWeapon)
		{
			SpawnDefaultWeapon();
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

				DamageCauser->GetInstigator()->PlayerState->Score += 1;
			}

			CurrentWeapon->RemoveWeapon();
			Destroy();

		}
	}
	return ActualDamage;
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

void ARCharacter::SpawnDefaultWeapon()
{
	if (Role < ROLE_Authority) { return; }

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ARWeapon* NewWeapon = GetWorld()->SpawnActor<ARWeapon>(DefaultWeapon, SpawnInfo);

	UE_LOG(LogTemp, Warning, TEXT("SpawnWeapon Completed"));

	EquipWeapon(NewWeapon);
}

void ARCharacter::EquipWeapon(ARWeapon* TheWeapon)
{
	//If The Weapon Is Valid
	if (TheWeapon)
	{
		if (Role == ROLE_Authority) { SetCurrentWeapon(TheWeapon); }
		else { ServerEquipWeapon(TheWeapon); }
	}
}

// Server Functions
void ARCharacter::ServerEquipWeapon_Implementation(ARWeapon* TheWeapon) { EquipWeapon(TheWeapon); }
bool ARCharacter::ServerEquipWeapon_Validate(ARWeapon* TheWeapon) { return true; }

//OnRep Function
void ARCharacter::OnRep_CurrentWeapon() 
{ 
	SetCurrentWeapon(CurrentWeapon); 
}

void ARCharacter::SetCurrentWeapon(class ARWeapon* NewWeapon)
{
	CurrentWeapon = NewWeapon;

	if (NewWeapon)
	{
		NewWeapon->SetOwningPawn(this);
		NewWeapon->OnEquip();
	}
}

FName ARCharacter::GetWeaponAttachPoint() { return WeaponAttachPoint; }

ARWeapon* ARCharacter::GetCurrentWeapon() { return CurrentWeapon; }

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