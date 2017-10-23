// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboHood.h"
#include "RWeapon.h"

//Other Classes
#include "RCharacter.h"
#include "RPlayerController.h"
#include "RProjectileBase.h"

//Other Includes
#include "Components/SceneComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"

//Default Constructor
ARWeapon::ARWeapon()
{
	//Set This Character To Call Tick() Every Frame.  You Can Turn This Off To Improve Performance If You Don't Need It.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	Mesh->CastShadow = true;
	Mesh->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	RootComponent = Mesh;

	bIsEquipped = false;
	bIsReloading = false;

	MaxAmmo = 20;
	CurrentAmmo = 20;

	RechargeWeapon();
}

void ARWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARWeapon, MyPawn);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// General Weapon Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Sets OwningPawn
void ARWeapon::SetOwningPawn(class ARCharacter* NewOwner)
{
	if (MyPawn != NewOwner)
	{
		Instigator = NewOwner;
		MyPawn = NewOwner;
		SetOwner(NewOwner);
	}
}

//Equips The Weapon
void ARWeapon::OnEquip()
{
	AttachMeshToPawn();

	bIsEquipped = true;

	DetermineWeaponState();
}

//Attaches Weapon To Pawn
void ARWeapon::AttachMeshToPawn()
{
	if (MyPawn)
	{
		USkeletalMeshComponent* PawnMesh = MyPawn->GetMesh();
		FName AttachPoint = MyPawn->GetWeaponAttachPoint();

		if (PawnMesh)
		{
			Mesh->AttachToComponent(PawnMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, AttachPoint);
		}
	}
}

//Destroys Weapob
void ARWeapon::RemoveWeapon()
{
	Destroy();
}

void ARWeapon::OnRep_MyPawn()
{
	if (MyPawn)
	{
		SetOwningPawn(MyPawn);
	}
	else { if (Role == ROLE_Authority) { SetOwningPawn(nullptr); } }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Weapon Shoot Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ARWeapon::StartFire()
{
	if (Role < ROLE_Authority) { ServerStartFire(); }

	if (!bWantsToFire)
	{
		bWantsToFire = true;
		DetermineWeaponState();
	}
}


void ARWeapon::StopFire()
{
	if (Role < ROLE_Authority) { ServerStopFire(); }

	if (bWantsToFire)
	{
		bWantsToFire = false;
		DetermineWeaponState();
	}
}

void ARWeapon::ServerStartFire_Implementation() { StartFire(); }

bool ARWeapon::ServerStartFire_Validate() { return true; }

void ARWeapon::ServerStopFire_Implementation() { StopFire(); }

bool ARWeapon::ServerStopFire_Validate() { return true; }

void ARWeapon::HandleFiring()
{
	if (CanFire())
	{
		if (MyPawn && MyPawn->IsLocallyControlled())
		{
			if (CurrentAmmo > 0)
			{
				FireWeapon();

				UseAmmo();
			}
			else
			{
				StartRecharge();
			}
		}
	}

	if (MyPawn && MyPawn->IsLocallyControlled())
	{
		if (Role < ROLE_Authority) { ServerHandleFiring(); }

		bRefiring = (CurrentState == EWeaponState::Firing && TimeBetweenShots > 0.0f);
		if (bRefiring)
		{
			GetWorldTimerManager().SetTimer(HandleFiringTimerHandle, this, &ARWeapon::HandleFiring, TimeBetweenShots, false);
		}
	}

	LastFireTime = GetWorld()->GetTimeSeconds();
}

void ARWeapon::ServerHandleFiring_Implementation() 
{ 

	const bool bShouldUpdateAmmo = (CurrentAmmo > 0 && CanFire());

	HandleFiring(); 

	if (bShouldUpdateAmmo)
	{
		UseAmmo();
	}
}

bool ARWeapon::ServerHandleFiring_Validate() { return true; }

//This Function Needs Alot OF CleanUp And Fixing
void ARWeapon::FireWeapon()
{
	if (Role < ROLE_Authority)
	{
		ServerFireWeapon();
		return;
	}

	//Variables
	FVector CameraLocation = MyPawn->GetFollowCamera()->ComponentToWorld.GetLocation();
	FRotator CameraRotation = MyPawn->GetFollowCamera()->ComponentToWorld.GetRotation().Rotator();

	FVector LineTraceStart = CameraLocation;
	FVector LineTraceEnd = (UKismetMathLibrary::GetForwardVector(CameraRotation) * 10000) + CameraLocation;

	//LineTrace Towards The Forward Vector Of The Camera Not The Player
	FHitResult LineTrace = WeaponTrace(LineTraceStart, LineTraceEnd);

	//Set Spawn Collision Handling Override
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ActorSpawnParams.Instigator = MyPawn;

	//This is the position on the muzzle
	FVector MuzzlePosition = Mesh->GetBoneLocation("joint2", EBoneSpaces::WorldSpace);

	if (LineTrace.bBlockingHit)
	{
		FVector InpactPoint = LineTrace.ImpactPoint;
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(MuzzlePosition, InpactPoint);
		GetWorld()->SpawnActor<ARProjectileBase>(Projectile, MuzzlePosition, LookAtRotation, ActorSpawnParams);

	}
	else
	{
		FVector LineTraceEndTwo = (UKismetMathLibrary::GetForwardVector(CameraRotation) * 3000) + CameraLocation;
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(MuzzlePosition, LineTraceEndTwo);
		GetWorld()->SpawnActor<ARProjectileBase>(Projectile, MuzzlePosition, LookAtRotation, ActorSpawnParams);

	}
}

void ARWeapon::ServerFireWeapon_Implementation() { FireWeapon(); }

bool ARWeapon::ServerFireWeapon_Validate() { return true; }

FHitResult ARWeapon::WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo) const
{
	FCollisionQueryParams TraceParams(TEXT("WeaponTrace"), true, Instigator);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, TraceFrom, TraceTo, ECC_WorldStatic, TraceParams);

	return Hit;
}

void ARWeapon::OnBurstStarted()
{
	const float GameTime = GetWorld()->GetTimeSeconds();

	if (LastFireTime > 0 && TimeBetweenShots > 0.0f && LastFireTime + TimeBetweenShots > GameTime)
	{
		GetWorldTimerManager().SetTimer(HandleFiringTimerHandle, this, &ARWeapon::HandleFiring, LastFireTime + TimeBetweenShots - GameTime, false);
	}
	else { HandleFiring(); }
}


void ARWeapon::OnBurstFinished()
{
	GetWorldTimerManager().ClearTimer(HandleFiringTimerHandle);
	bRefiring = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Check State Of The Weapon Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ARWeapon::CanFire() const
{
	bool bPawnCanFire = MyPawn;
	bool bStateOK = CurrentState == EWeaponState::Idle || CurrentState == EWeaponState::Firing;
	return bPawnCanFire && bStateOK;
}

void ARWeapon::DetermineWeaponState()
{
	EWeaponState NewState = EWeaponState::Idle;

	if (bIsEquipped)
	{
		if (bIsReloading)
		{
			NewState = EWeaponState::Recharging;
		}
		else if (bWantsToFire && CanFire())
		{
			NewState = EWeaponState::Firing;
		}
	}

	SetWeaponState(NewState);
}

void ARWeapon::SetWeaponState(EWeaponState NewState)
{
	const EWeaponState PrevState = CurrentState;

	if (PrevState == EWeaponState::Firing && NewState != EWeaponState::Firing)
	{
		OnBurstFinished();
	}

	CurrentState = NewState;

	if (PrevState != EWeaponState::Firing && NewState == EWeaponState::Firing)
	{
		OnBurstStarted();
	}
}

EWeaponState ARWeapon::GetCurrentState() const
{
	return CurrentState;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Weapon Ammo Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ARWeapon::UseAmmo()
{
	CurrentAmmo--;

	UE_LOG(LogTemp, Warning, TEXT("CurrentAmmo: %d"), CurrentAmmo);
}

void ARWeapon::StartRecharge()
{
	bIsReloading = true;
	DetermineWeaponState();

	GetWorldTimerManager().SetTimer(TimerHandle_ReloadWeapon, this, &ARWeapon::RechargeWeapon, ReloadWeaponTime, false);
}

void ARWeapon::StopRecharge()
{
	bIsReloading = false;
	DetermineWeaponState();
}

void ARWeapon::RechargeWeapon()
{
	CurrentAmmo = MaxAmmo;

	StopRecharge();
}