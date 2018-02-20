// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboHood.h"
#include "RWeapon.h"

//Other Classes
#include "RCharacter.h"
#include "RPlayerController.h"
#include "RProjectileBase.h"

//Other Includes
#include "Components/SceneComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "RCameraManager.h"

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
}

void ARWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	MaxAmmo = MaxAmmo;
	CurrentAmmo = StartAmmo;
}

void ARWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARWeapon, MyPawn);

	DOREPLIFETIME(ARWeapon, MFlash);
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
	AttachMeshToPawn("Hand");
	bIsEquipped = true;
	DetermineWeaponState();
}

void ARWeapon::OnUnEquip()
{
	DeAttachMeshToPawn();
	bIsEquipped = false;
	StopFire();
	DetermineWeaponState();
}

//Attaches Weapon To Pawn
void ARWeapon::AttachMeshToPawn(FName SocketName)
{
	if (MyPawn)
	{
		USkeletalMeshComponent* PawnMesh = MyPawn->GetMesh();
		FName AttachPoint = MyPawn->GetWeaponAttachPoint();

		if (PawnMesh)
		{
			Mesh->AttachToComponent(PawnMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
			if (SocketName == "Storage")
				Mesh->SetHiddenInGame(true);
			else
				Mesh->SetHiddenInGame(false);
		}
	}
}

void ARWeapon::DeAttachMeshToPawn()
{
	Mesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
}

void ARWeapon::OnEnterInventory(ARCharacter* NewOwner)
{
	SetOwningPawn(NewOwner);
	AttachMeshToPawn("Storage");
	Mesh->SetHiddenInGame(true);
}

void ARWeapon::OnRep_MyPawn()
{
	if (MyPawn)
	{
		OnEnterInventory(MyPawn);
	}
}

//Destroys Weapob
void ARWeapon::RemoveWeapon()
{
	Destroy();
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
				if (bCanReload)
				{
					StartRecharge();
				}
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

	FVector ShootDir = GetAdjustedAim();
	FVector Origin = GetMuzzleLocation();

	const float ProjectileAdjustRange = 15000.0f;
	const FVector StartTrace = GetCameraDamageStartLocation(ShootDir);
	const FVector EndTrace = StartTrace + ShootDir * ProjectileAdjustRange;
	FHitResult Impact = WeaponTrace(StartTrace, EndTrace);

	// and adjust directions to hit that actor
	if (Impact.bBlockingHit)
	{
		const FVector AdjustedDir = (Impact.ImpactPoint - Origin).GetSafeNormal();
		bool bWeaponPenetration = false;

		const float DirectionDot = FVector::DotProduct(AdjustedDir, ShootDir);
		if (DirectionDot < 0.0f)
		{
			// shooting backwards = weapon is penetrating
			bWeaponPenetration = true;
		}
		else if (DirectionDot < 0.5f)
		{
			// check for weapon penetration if angle difference is big enough
			// raycast along weapon mesh to check if there's blocking hit

			FVector MuzzleStartTrace = Origin - GetMuzzleDirection() * 150.0f;
			FVector MuzzleEndTrace = Origin;
			FHitResult MuzzleImpact = WeaponTrace(MuzzleStartTrace, MuzzleEndTrace);

			if (MuzzleImpact.bBlockingHit)
			{
				bWeaponPenetration = true;
			}
		}

		if (bWeaponPenetration)
		{
			Origin = Impact.ImpactPoint - ShootDir * 10.0f;
		}
		else
		{
			// adjust direction to hit
			ShootDir = AdjustedDir;
		}
	}

	if (MuzzleFlash) SpawnMuzzleFlash();
	MFlash = !MFlash;
	
	SpawnProjectile(Origin, ShootDir);


	//UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());
	//if (Primitive && Proj) Primitive->IgnoreActorWhenMoving(Proj, true);

	
}

void ARWeapon::SpawnProjectile(FVector Origin, FVector ShootDir)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = Instigator;

	if (ProjectileBP)
		ARProjectileBase* Proj = GetWorld()->SpawnActor<ARProjectileBase>(ProjectileBP, Origin, ShootDir.Rotation(), SpawnParams);
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

	DrawDebugLine(GetWorld(), TraceFrom, Hit.ImpactPoint, FColor(255, 0, 0), false, 5, 0, 4);
	DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 48, 12, FColor(255, 0, 0), false, 5, 0, 4);

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
	if (bCanReload)
	{
		bIsReloading = true;
		DetermineWeaponState();

		GetWorldTimerManager().SetTimer(TimerHandle_ReloadWeapon, this, &ARWeapon::RechargeWeapon, ReloadWeaponTime, false);
	}
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

void ARWeapon::IncreaseAmmo()
{
	CurrentAmmo += 15;
}

void ARWeapon::SpawnMuzzleFlash()
{
	//This is the position on the muzzle
	FRotator Rotation = FRotator(90, 90, 0);

	if (MuzzleFlash)
		UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, Mesh, MuzzleBone, FVector::ZeroVector, Rotation, EAttachLocation::SnapToTarget);
}

void ARWeapon::OnRep_MFlash()
{
	if (Role < ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("Client"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Server"));
	}

	SpawnMuzzleFlash();

}

//Aim Function

FVector ARWeapon::GetAdjustedAim()
{
	ARPlayerController* const PlayerController = Instigator ? Cast<ARPlayerController>(Instigator->Controller) : NULL;

	FVector FinalAim = FVector::ZeroVector;
	// If we have a player controller use it for the aim
	if (PlayerController)
	{
		FVector CamLoc;
		FRotator CamRot;
		PlayerController->GetPlayerViewPoint(CamLoc, CamRot);
		FinalAim = CamRot.Vector();
	}
	else if (Instigator)
	{
		FinalAim = Instigator->GetBaseAimRotation().Vector();
	}

	return FinalAim;
}

FVector ARWeapon::GetCameraDamageStartLocation(const FVector& AimDir)
{
	ARPlayerController* PC = MyPawn ? Cast<ARPlayerController>(MyPawn->Controller) : NULL;
	FVector OutStartTrace = FVector::ZeroVector;

	if (PC)
	{
		// use player's camera
		FRotator UnusedRot;
		PC->GetPlayerViewPoint(OutStartTrace, UnusedRot);

		// Adjust trace so there is nothing blocking the ray between the camera and the pawn, and calculate distance from adjusted start
		OutStartTrace = OutStartTrace + AimDir * ((Instigator->GetActorLocation() - OutStartTrace) | AimDir);
	}

	return OutStartTrace;
}

FVector ARWeapon::GetMuzzleLocation()
{
	return Mesh->GetSocketLocation(MuzzleBone);
}

FVector ARWeapon::GetMuzzleDirection()
{
	return Mesh->GetSocketRotation(MuzzleBone).Vector();
}