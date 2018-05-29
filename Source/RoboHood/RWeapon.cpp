// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboHood.h"
#include "RWeapon.h"

//Other Classes
#include "RCharacter.h"
#include "RPlayerController.h"
#include "RProjectileBase.h"

//Other Includes
#include "Math/UnrealMathUtility.h"
#include "Sound/SoundCue.h"
#include "Components/SceneComponent.h"
#include "Components/PrimitiveComponent.h"
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
}

void ARWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	CurrentAmmo = AmmoPerClip;
	if (bUseDynamicMat) DynamicMat = Mesh->CreateAndSetMaterialInstanceDynamic(0);
}

void ARWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARWeapon, MyPawn);

	DOREPLIFETIME_CONDITION(ARWeapon, CurrentAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ARWeapon, BurstCounter, COND_SkipOwner);
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

	StopFire();
	bIsEquipped = false;
	DetermineWeaponState();
}

//Attaches Weapon To Pawn
void ARWeapon::AttachMeshToPawn(FName SocketName)
{
	if (MyPawn)
	{

		DeAttachMeshToPawn();

		USkeletalMeshComponent* PawnMesh = MyPawn->GetMesh();

		FName AttachPoint = MyPawn->GetWeaponAttachPoint();

		if (MyPawn->IsLocallyControlled() == true)
		{
			Mesh->SetHiddenInGame(false);
			Mesh->AttachToComponent(PawnMesh, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);

		}
		else
		{
			Mesh->AttachToComponent(PawnMesh, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
			Mesh->SetHiddenInGame(false);
		}
	}
}

void ARWeapon::DeAttachMeshToPawn()
{
	Mesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	Mesh->SetHiddenInGame(true);
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

void ARWeapon::ServerStartFire_Implementation() 
{ 
	StartFire(); 
}

void ARWeapon::ServerStopFire_Implementation() 
{ 
	StopFire(); 
}

void ARWeapon::HandleFiring()
{
	if (CanFire() && CurrentAmmo > 0)
	{

		if (GetNetMode() != NM_DedicatedServer)
		{
			SpawnMuzzleFlash();
			PlayShootSound();
		}

		if (MyPawn && MyPawn->IsLocallyControlled())
		{
			FireWeapon();

			UseAmmo();

			BurstCounter++;
		}
	}
	else if (bCanReload)
	{
		StartRecharge();
	}
	else if (MyPawn && MyPawn->IsLocallyControlled())
	{
		if (BurstCounter > 0)
		{
			OnBurstFinished();
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

		BurstCounter++;
	}
}

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

	//And adjust directions to hit that actor
	if (Impact.bBlockingHit)
	{
		const FVector AdjustedDir = (Impact.ImpactPoint - Origin).GetSafeNormal();
		bool bWeaponPenetration = false;

		const float DirectionDot = FVector::DotProduct(AdjustedDir, ShootDir);
		if (DirectionDot < 0.0f)
		{
			//Shooting backwards = weapon is penetrating
			bWeaponPenetration = true;
		}
		else if (DirectionDot < 0.5f)
		{
			//Check for weapon penetration if angle difference is big enough
			//Raycast along weapon mesh to check if there's blocking hit
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
			//Adjust direction to hit
			ShootDir = AdjustedDir;
		}
	}
	
	SpawnProjectile(Origin, ShootDir);	
}

void ARWeapon::SpawnProjectile(FVector Origin, FVector ShootDir)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = Instigator;

	if (ProjectileBP != NULL)
	{
		ARProjectileBase* Proj = GetWorld()->SpawnActor<ARProjectileBase>(ProjectileBP, Origin, ShootDir.Rotation(), SpawnParams);
	}
}

void ARWeapon::ServerFireWeapon_Implementation() 
{
	FireWeapon(); 
}

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
	BurstCounter = 0;
	if (bUseDynamicMat) IncreaseWeaponEmissive(false);

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
}

void ARWeapon::StartRecharge()
{
	if (Role < ROLE_Authority)
	{
		ServerStartRecharge();
	}

	if (bCanReload && CurrentAmmo != AmmoPerClip && !bIsReloading)
	{
		bIsReloading = true;
		DetermineWeaponState();
		PlayReloadSound(ReloadSound);

		GetWorldTimerManager().SetTimer(TimerHandle_ReloadWeapon, this, &ARWeapon::RechargeWeapon, ReloadWeaponTime, false);
	}
}

void ARWeapon::PlayReloadSound_Implementation(USoundCue* Sound)
{
	if (Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, GetActorLocation());
	}
}

void ARWeapon::ServerStartRecharge_Implementation()
{
	StartRecharge();
}

void ARWeapon::StopRecharge()
{
	bIsReloading = false;
	DetermineWeaponState();
}

void ARWeapon::RechargeWeapon()
{
	CurrentAmmo = AmmoPerClip;

	StopRecharge();
}

void ARWeapon::IncreaseAmmo(int32 Ammount)
{
	CurrentAmmo += Ammount;
}

void ARWeapon::SpawnMuzzleFlash()
{

	if (Role == ROLE_Authority && CurrentState != EWeaponState::Firing)
	{
		return;
	}

	if (MuzzleFlash_Particle)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleFlash_Particle, Mesh, MuzzleBone_Name, FVector::ZeroVector, MuzzleRotation, EAttachLocation::SnapToTarget);
	}

	//T8-03 Specific
	if (ShootAnimation) PlayWeaponAnimation(ShootAnimation);
	if (bUseDynamicMat) IncreaseWeaponEmissive(true);
}

void ARWeapon::PlayWeaponAnimation(UAnimationAsset* Animation)
{
	if (Mesh)
	{
		if (Animation)
		{
			Mesh->PlayAnimation(Animation, false);
		}
	}
}

void ARWeapon::PlayShootSound_Implementation()
{
	if (GetDefaultWeapon())
	{
		int index = FMath::RandRange(0, 2);
		if (ShootSounds[index])
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShootSounds[index], GetMuzzleLocation());
		}
	}
	else
	{
		if (Explosion_Rocket_ShootSounds)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), Explosion_Rocket_ShootSounds, GetMuzzleLocation());
		}
	}
}

void ARWeapon::OnRep_BurstCounter()
{
	if (BurstCounter > 0)
	{
		SpawnMuzzleFlash();
		PlayShootSound();
	}

	if (!(BurstCounter > 0) && bUseDynamicMat)
	{
		IncreaseWeaponEmissive(true);
	}
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
	return Mesh->GetSocketLocation(MuzzleBone_Name);
}

FVector ARWeapon::GetMuzzleDirection()
{
	return Mesh->GetSocketRotation(MuzzleBone_Name).Vector();
}

void ARWeapon::IncreaseWeaponEmissive(bool bIncrease)
{
	if (bUseDynamicMat)
	{
		float Value = E_BaseValue + E_Increase * (BurstCounter + (bIncrease ? 1 : 0));
		DynamicMat->SetScalarParameterValue(FName("Intensity"), Value);

		//UE_LOG(LogTemp, Warning, TEXT("Num: %d"), BurstCounter + (bIncrease ? 1 : 0));
	}
}