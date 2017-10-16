// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboHood.h"
#include "Weapon.h"
#include "TestCharacter.h"
#include "MyPlayerController.h"
#include "FireBall.h"

#include "Components/SceneComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh3P"));
	Mesh->CastShadow = true;
	Mesh->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	RootComponent = Mesh;

	bIsEquipped = false;
}


void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, MyPawn);
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::SetOwningPawn(class ATestCharacter* NewOwner)
{

	if (MyPawn != NewOwner)
	{
		Instigator = NewOwner;
		MyPawn = NewOwner;
		SetOwner(NewOwner);
	}
}

void AWeapon::OnRep_MyPawn()
{
	if (MyPawn)
	{
		SetOwningPawn(MyPawn);
	}
	else
	{
		if (Role == ROLE_Authority)
		{
			SetOwningPawn(nullptr);
		}
	}
}

void AWeapon::AttachMeshToPawn()
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

void AWeapon::RemoveWeapon()
{
	Destroy();
}

void AWeapon::OnEquip()
{
	AttachMeshToPawn();

	bIsEquipped = true;

	DetermineWeaponState();
}




















void AWeapon::StartFire()
{
	if (Role < ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("StartFire SERVER FUNCTION"));
		ServerStartFire();
	}

	if (!bWantsToFire)
	{
		UE_LOG(LogTemp, Warning, TEXT("StartFire LOCAL FUNCTION"));
		bWantsToFire = true;
		DetermineWeaponState();
	}
}


void AWeapon::StopFire()
{
	if (Role < ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("StopFire SERVER FUNCTION"));
		ServerStopFire();
	}

	if (bWantsToFire)
	{
		UE_LOG(LogTemp, Warning, TEXT("StopFire LOCAL FUNCTION"));
		bWantsToFire = false;
		DetermineWeaponState();
	}
}

void AWeapon::ServerStartFire_Implementation()
{
	StartFire();
}

bool AWeapon::ServerStartFire_Validate()
{
	return true;
}

void AWeapon::ServerStopFire_Implementation()
{
	StopFire();
}

bool AWeapon::ServerStopFire_Validate()
{
	return true;
}

bool AWeapon::CanFire() const
{
	bool bPawnCanFire = MyPawn;
	bool bStateOK = CurrentState == EWeaponState::Idle || CurrentState == EWeaponState::Firing;
	return bPawnCanFire && bStateOK;
}

void AWeapon::HandleFiring()
{
	if (CanFire())
	{
		if (MyPawn && MyPawn->IsLocallyControlled())
		{
			UE_LOG(LogTemp, Warning, TEXT("HandleFiring LOCAL FUNCTION"));
			FireWeapon();
		}
	}
	
	if (MyPawn && MyPawn->IsLocallyControlled())
	{
		if (Role < ROLE_Authority)
		{
			UE_LOG(LogTemp, Warning, TEXT("HandleFiring SERVER FUNCTION"));
			ServerHandleFiring();
		}

		bRefiring = (CurrentState == EWeaponState::Firing && TimeBetweenShots > 0.0f);
		if (bRefiring)
		{
			GetWorldTimerManager().SetTimer(HandleFiringTimerHandle, this, &AWeapon::HandleFiring, TimeBetweenShots, false);
		}
	}

	LastFireTime = GetWorld()->GetTimeSeconds();
}

void AWeapon::ServerHandleFiring_Implementation()
{
	HandleFiring();
}

bool AWeapon::ServerHandleFiring_Validate()
{
	return true;
}

void AWeapon::FireWeapon()
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
	
	const FRotator SpawnRotation = MyPawn->GetControlRotation();
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

void AWeapon::ServerFireWeapon_Implementation()
{
	FireWeapon();
}

bool AWeapon::ServerFireWeapon_Validate()
{
	return true;
}

FHitResult AWeapon::WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo) const
{
	FCollisionQueryParams TraceParams(TEXT("WeaponTrace"), true, Instigator);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, TraceFrom, TraceTo, ECC_WorldStatic, TraceParams);

	return Hit;
}




























//Weapon State Functions
void AWeapon::DetermineWeaponState()
{
	EWeaponState NewState = EWeaponState::Idle;

	if (bIsEquipped)
	{
		if (bWantsToFire && CanFire())
		{
			NewState = EWeaponState::Firing;
		}
	}

	SetWeaponState(NewState);
}

void AWeapon::SetWeaponState(EWeaponState NewState)
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

EWeaponState AWeapon::GetCurrentState() const
{
	return CurrentState;
}





void AWeapon::OnBurstStarted()
{
	// Start firing, can be delayed to satisfy TimeBetweenShots
	const float GameTime = GetWorld()->GetTimeSeconds();

	if (LastFireTime > 0 && TimeBetweenShots > 0.0f && LastFireTime + TimeBetweenShots > GameTime)
	{
		GetWorldTimerManager().SetTimer(HandleFiringTimerHandle, this, &AWeapon::HandleFiring, LastFireTime + TimeBetweenShots - GameTime, false);
	}
	else
	{
		HandleFiring();
	}
}


void AWeapon::OnBurstFinished()
{
	GetWorldTimerManager().ClearTimer(HandleFiringTimerHandle);
	bRefiring = false;
}
