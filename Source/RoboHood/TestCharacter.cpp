// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboHood.h"
#include "TestCharacter.h"
#include "FireBall.h"
#include "MyPlayerController.h"

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

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ATestCharacter::OnHit);

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

	//FireBall Constructor
	static ConstructorHelpers::FClassFinder<AFireBall> FireBallProjectile(TEXT("/Game/MyFireBall"));
	if (FireBallProjectile.Succeeded()) { FireBall = FireBallProjectile.Class; }

	//Explode SFX COnstructor
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ExplosionEffect (TEXT("/Game/StarterContent/Particles/P_Explosion"));
	if (ExplosionEffect.Succeeded()) { ExplodeParticleSystem = ExplosionEffect.Object; }

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
	PlayerInputComponent->BindAction("LeftMouse", IE_Pressed, this, &ATestCharacter::ExplodeON);
	PlayerInputComponent->BindAction("LeftMouse", IE_Released, this, &ATestCharacter::ExplodeOFF);

	//Fireball Input
	PlayerInputComponent->BindAction("RightMouse", IE_Pressed, this, &ATestCharacter::SpawnFireBall);
}

//This Function Is Part of the multiplayer framework and replicated variables between server and clients
void ATestCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATestCharacter, Explode);
	
	DOREPLIFETIME(ATestCharacter, Health);
}


void ATestCharacter::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
}

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

				UE_LOG(LogTemp, Warning, TEXT("Killed"));
			}

			Destroy();
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("CurrentHealth %f"), Health);

	return ActualDamage;
}

void ATestCharacter::ExplodeON()
{
	Explode = true;
	OnRep_Explode();
}

void ATestCharacter::ExplodeOFF()
{
	Explode = false;
	OnRep_Explode();
}

void ATestCharacter::PlayerExplode()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplodeParticleSystem, GetActorTransform(), true);

	if (Role < ROLE_Authority) { ServerExplode(); }
}

void ATestCharacter::ServerExplode_Implementation() { PlayerExplode(); }
bool ATestCharacter::ServerExplode_Validate() { return true; }

//I think this function is what tells the server to duplicate the variable maybe?
void ATestCharacter::OnRep_Explode()
{
	if (Explode)
	{
		PlayerExplode();
	}
}

void ATestCharacter::SpawnFireBall()
{
	// try and fire a projectile
	if (Role < ROLE_Authority)
	{
		ServerSpawnFireBall();
		return;
	}

	//OK So Here A Need To Do Some BS Calculations To Shoot Towards To CrossHair :/

	//Variables
	FVector CameraLocation = FollowCamera->ComponentToWorld.GetLocation();
	FRotator CameraRotation = FollowCamera->ComponentToWorld.GetRotation().Rotator();

	FVector LineTraceStart = CameraLocation;
	FVector LineTraceEnd = (UKismetMathLibrary::GetForwardVector(CameraRotation) * 5000) + CameraLocation;

	//LineTrace Towards The Forward Vector Of The Camera Not The Player
	FHitResult LineTrace = WeaponTrace(LineTraceStart, LineTraceEnd);

	//Set Spawn Collision Handling Override
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	const FRotator SpawnRotation = GetControlRotation();
	const FVector SpawnLocation = GetActorLocation();
	const FVector ForwardVec = GetActorForwardVector();
	const FVector OffSet = (ForwardVec * 60) + (SpawnLocation + FVector(0, 0, 50));

	if (LineTrace.bBlockingHit)
	{
		FVector InpactPoint = LineTrace.ImpactPoint;
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(OffSet, InpactPoint);

		// spawn the projectile at the muzzle
		GetWorld()->SpawnActor<AFireBall>(FireBall, OffSet, LookAtRotation, ActorSpawnParams);
	}
	else
	{
		FVector LineTraceEndTwo = (UKismetMathLibrary::GetForwardVector(CameraRotation) * 3000) + CameraLocation;
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(OffSet, LineTraceEndTwo);

		// spawn the projectile at the muzzle
		GetWorld()->SpawnActor<AFireBall>(FireBall, OffSet, LookAtRotation, ActorSpawnParams);
	}
			
}

FHitResult ATestCharacter::WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo) const
{
	FCollisionQueryParams TraceParams(TEXT("WeaponTrace"), true, Instigator);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, TraceFrom, TraceTo, ECC_WorldStatic, TraceParams);

	return Hit;
}

void ATestCharacter::ServerSpawnFireBall_Implementation() { SpawnFireBall(); }
bool ATestCharacter::ServerSpawnFireBall_Validate() { return true; }

//-----------------------------------------------------------------------------------------------------//

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Default Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////

// Called when the game starts or when spawned
void ATestCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATestCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

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
