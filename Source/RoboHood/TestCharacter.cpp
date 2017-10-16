// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboHood.h"
#include "TestCharacter.h"
#include "FireBall.h"
#include "MyPlayerController.h"
#include "Weapon.h"

#include "Net/UnrealNetwork.h"

#include "GameFramework/Actor.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ATestCharacter::ATestCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Health = 100.f;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	bReplicates = true;

}

// Called to bind functionality to input
void ATestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up gameplay key bindings
	check(PlayerInputComponent);

	//Jump Input
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	//Movement Input
	PlayerInputComponent->BindAxis("MoveForward", this, &ATestCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATestCharacter::MoveRight);

	//Camera Input
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ATestCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ATestCharacter::LookUpAtRate);

	//Explode Input
	PlayerInputComponent->BindAction("LeftMouse", IE_Pressed, this, &ATestCharacter::OnStartFire);
	PlayerInputComponent->BindAction("LeftMouse", IE_Released, this, &ATestCharacter::OnStopFire);

	//Fireball Input
	//PlayerInputComponent->BindAction("RightMouse", IE_Pressed, this, &ATestCharacter::BeginFire);
	//PlayerInputComponent->BindAction("RightMouse", IE_Released, this, &ATestCharacter::EndFire);
}

//This Function Is Part of the multiplayer framework and replicated variables between server and clients
void ATestCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ATestCharacter, Health);

	DOREPLIFETIME(ATestCharacter, CurrentWeapon);

}

void ATestCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UE_LOG(LogTemp, Warning, TEXT("PostInit Called"));

	if (Role == ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnWeapon Called"));
		SpawnDefaultWeapon();
	}
}

void ATestCharacter::SpawnDefaultWeapon()
{
	if (Role < ROLE_Authority)
	{
		return;
	}

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AWeapon* NewWeapon = GetWorld()->SpawnActor<AWeapon>(DefaultWeapon, SpawnInfo);

	UE_LOG(LogTemp, Warning, TEXT("SpawnWeapon Completed"));

	EquipWeapon(NewWeapon);

}

void ATestCharacter::EquipWeapon(AWeapon* TheWeapon)
{
	if (TheWeapon)
	{
		if (Role == ROLE_Authority)
		{
			SetCurrentWeapon(TheWeapon);
		}
		else
		{
			ServerEquipWeapon(TheWeapon);
		}
	}
}

void ATestCharacter::ServerEquipWeapon_Implementation(AWeapon* TheWeapon)
{
	EquipWeapon(TheWeapon);
}

bool ATestCharacter::ServerEquipWeapon_Validate(AWeapon* TheWeapon)
{
	return true;
}

void ATestCharacter::OnRep_CurrentWeapon()
{
	SetCurrentWeapon(CurrentWeapon);
}

void ATestCharacter::SetCurrentWeapon(class AWeapon* NewWeapon)
{
	CurrentWeapon = NewWeapon;

	if (NewWeapon)
	{
		//Need To Implament These
		NewWeapon->SetOwningPawn(this);
	    NewWeapon->OnEquip();
	}
}

FName ATestCharacter::GetWeaponAttachPoint()
{
	return WeaponAttachPoint;
}


void ATestCharacter::OnStartFire()
{
	StartWeaponFire();
}


void ATestCharacter::OnStopFire()
{
	StopWeaponFire();
}


void ATestCharacter::StartWeaponFire()
{
	if (!bWantsToFire)
	{
		bWantsToFire = true;
		if (CurrentWeapon)
		{
			CurrentWeapon->StartFire();
		}
	}
}


void ATestCharacter::StopWeaponFire()
{
	if (bWantsToFire)
	{
		bWantsToFire = false;
		if (CurrentWeapon)
		{
			CurrentWeapon->StopFire();
		}
	}
}

//-----------------------------------------------------------------------------------------------------//

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Movement Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////

//Camera Left/Right
void ATestCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

//Camera Up/Down
void ATestCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

//Move Forward/Back Function
void ATestCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

//Move Left/Right Function
void ATestCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

// This is the actor damage handler.   
float ATestCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	// Call the base class - this will tell us how much damage to apply  
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f)
	{
		Health -= ActualDamage;
		// If the damage depletes our health set our lifespan to zero - which will destroy the actor  
		if (Health <= 0.f)
		{
			AMyPlayerController* PlayerController = Cast<AMyPlayerController>(Controller);
			if (PlayerController)
			{
				PlayerController->OnKilled();
			}

			CurrentWeapon->RemoveWeapon();
			Destroy();

		}
	}
	return ActualDamage;
}

//void ATestCharacter::ExplodeON()
//{
//	ServerExplode();
//}
//
//void ATestCharacter::ExplodeOFF()
//{
//	ServerExplode();
//}
//
//void ATestCharacter::ServerExplode_Implementation()
//{
//	Explode = !Explode;
//	OnRep_Explode();
//}
//bool ATestCharacter::ServerExplode_Validate() { return true; }
//
////I think this function is what tells the server to duplicate the variable maybe?
//void ATestCharacter::OnRep_Explode()
//{
//	if (Explode)
//	{
//		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Particle, GetActorTransform(), true);
//	}
//}

