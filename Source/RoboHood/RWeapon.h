// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GameFramework/Actor.h"
#include "RWeapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	Idle,
	Firing,
	Recharging
};

UCLASS()
class ROBOHOOD_API ARWeapon : public AActor
{
	GENERATED_BODY()

	//Default Constructor.
	ARWeapon();

	virtual void PostInitializeComponents() override;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Basic Weapon Functions And Variables
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
public:	

	//Is This The Default Weapon?.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Info")
	bool bDefaultWeapon;

	UFUNCTION(BlueprintCallable)
	bool GetDefaultWeapon() { return bDefaultWeapon; }

	//Projectile.
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	TSubclassOf<class ARProjectileBase> ProjectileBP;

	//FireRate.
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	float TimeBetweenShots;

	//OnHit Particle Effect
	UPROPERTY(EditDefaultsOnly, Category = "Projectile Attributes", Meta = (BlueprintProtected = "true"))
	UParticleSystem* MuzzleFlash;

public:

	//Called From Player Sets MyPawn
	void SetOwningPawn(class ARCharacter* NewOwner);

	//Attaches Weapon To Player And Sets Equipped To True.
	void OnEquip();

	//Attaches Weapon To Player And Sets Equipped To True.
	void OnUnEquip();

	//Destroys The Weapno When Player Dies
	void RemoveWeapon();

	void OnEnterInventory(ARCharacter* NewOwner);

	//Attaches Weapon To Player Socket
	void AttachMeshToPawn(FName SocketName);

	//Attaches Weapon To Player Socket
	void DeAttachMeshToPawn();

protected:

	//This Is The Owner Of The Weapon
	UPROPERTY(Transient, ReplicatedUsing = OnRep_MyPawn)
	class ARCharacter* MyPawn;

	//This Is The Mesh For The Weapon (Change To SkeletalMesh When We Get Assets)
	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh")
	USkeletalMeshComponent* Mesh;

	//OnRep Function
	UFUNCTION()
	void OnRep_MyPawn();

private:

	//Multiplayer Framework For Replicating Variables
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	//Handles TimeBetween Shots.
	FTimerHandle HandleFiringTimerHandle;

	//Is The Weapon Equipped By The Player.
	bool bIsEquipped;

	//Is The Weapon Going To FireAgain?
	bool bRefiring;

	//Does The Player Want To Fire.
	bool bWantsToFire;

	//Get LastShot Time To Calculate TimeBetweenShots
	float LastFireTime;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Weapon Shoot Functions And Variables
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	//Fire Functions
	void StartFire();
	void StopFire();

private:

	//Server Function For StartFire
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerStartFire();
	void ServerStartFire_Implementation();
	bool ServerStartFire_Validate();

	//Server Function For StopFire
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerStopFire();
	void ServerStopFire_Implementation();
	bool ServerStopFire_Validate();

	//Handle Fireing
	void HandleFiring();

	//HandleFiring Server Function
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerHandleFiring();
	void ServerHandleFiring_Implementation();
	bool ServerHandleFiring_Validate();

	//Calculate Projectile Trajectory
	void FireWeapon();

	//Spawns The Projectile
	void SpawnProjectile(FVector Origin, FVector ShootDir);

	//FireWeapon Server Function
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerFireWeapon();
	void ServerFireWeapon_Implementation();
	bool ServerFireWeapon_Validate();

	//Does A RayCast For The Weapon
	FHitResult WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo) const;

	//Checks Stuff Before Firing
	void OnBurstStarted();
	void OnBurstFinished();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Check State Of The Weapon Functions And Variables
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

private:

	//Weapon Current State
	EWeaponState CurrentState;

	//Getter
	EWeaponState GetCurrentState() const;

	//Set New Weapon State
	void SetWeaponState(EWeaponState NewState);

	//Determine The Weapon State And Do Stuff
	void DetermineWeaponState();

	//Checks If The Player Can Fire
	bool CanFire() const;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Weapon Ammo Functions And Variables
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	virtual void StartRecharge();

	// - AMMO ----------------------------------------------------------
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	int32 StartAmmo;

	//MaxHeat
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Info")
	int32 MaxAmmo;
	// -----------------------------------------------------------------

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	bool bCanReload;

	//ReloadTime
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Info")
	float ReloadWeaponTime;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	FName MuzzleBone;

	//Weapons Current Ammo Replicated
	UPROPERTY(Transient, Replicated, BlueprintReadOnly)
	int32 CurrentAmmo;

	UFUNCTION(BlueprintCallable)
	int32 GetCurrentAmmo() { return CurrentAmmo; }

	UPROPERTY(BlueprintReadOnly)
	bool bIsReloading;

	UPROPERTY(BlueprintReadOnly)
	FTimerHandle TimerHandle_ReloadWeapon;

	void IncreaseAmmo();

	void SpawnMuzzleFlash();

	UPROPERTY(ReplicatedUsing = OnRep_MFlash)
	bool MFlash;

	UFUNCTION()
	void OnRep_MFlash();

protected:

	void UseAmmo();

	void StopRecharge();

	void RechargeWeapon();

public:

	FVector GetAdjustedAim();

	FVector GetCameraDamageStartLocation(const FVector& AimDir);

	FVector GetMuzzleLocation();

	FVector GetMuzzleDirection();
	
};
