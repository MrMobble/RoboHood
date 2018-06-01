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

	//Projectile.
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Settings")
	TSubclassOf<class ARProjectileBase> ProjectileBP;

	//MainGun Array Sounds.
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Settings")
	TArray<USoundCue*> ShootSounds;

	//Other Gun Shoot Sounds.
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Settings")
	USoundCue* Explosion_Rocket_ShootSounds;

	//ShootingSound.
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Settings")
	USoundCue* ReloadSound;

	//FireRate.
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Settings")
	float TimeBetweenShots;

	//Shoot Animation.
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Settings")
	UAnimationAsset* ShootAnimation;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Settings")
	FRotator MuzzleRotation;

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

	void PlayWeaponAnimation(UAnimationAsset* Animation);

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

	void ShootSound();

	UFUNCTION(NetMultiCast, Reliable)
	void PlayShootSound();
	void PlayShootSound_Implementation();

	UFUNCTION(NetMultiCast, Reliable)
	void PlayReloadSound(USoundCue* Sound);
	void PlayReloadSound_Implementation(USoundCue* Sound);

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

public:

	//Weapon Material T8-03 Specific
	UMaterialInstanceDynamic* DynamicMat;

	//This is T8-03
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Material")
	bool bUseDynamicMat;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Material")
	float E_BaseValue;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Material")
	float E_Increase;

	void IncreaseWeaponEmissive(bool bIncrease);

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
	bool ServerStartFire_Validate() { return true; }

	//Server Function For StopFire
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerStopFire();
	void ServerStopFire_Implementation();
	bool ServerStopFire_Validate() { return true; }

	//Handle Fireing
	void HandleFiring();

	//HandleFiring Server Function
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerHandleFiring();
	void ServerHandleFiring_Implementation();
	bool ServerHandleFiring_Validate() { return true; }

	//Calculate Projectile Trajectory
	void FireWeapon();

	//Spawns The Projectile
	void SpawnProjectile(FVector Origin, FVector ShootDir);

	//FireWeapon Server Function
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerFireWeapon();
	void ServerFireWeapon_Implementation();
	bool ServerFireWeapon_Validate() { return true; }

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

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Ammo

	UPROPERTY(Transient, Replicated)
	int32 CurrentAmmo;

	////////////////////////////////////////////////////////////////
	// Weapon Propertys

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Settings")
	int32 AmmoPerClip;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Settings")
	bool bCanReload;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Settings")
	float ReloadWeaponTime;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Settings")
	FName MuzzleBone_Name;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Settings")
	UParticleSystem* MuzzleFlash_Particle;

	//
	////////////////////////////////////////////////////////////////

	bool bIsReloading;

	FTimerHandle TimerHandle_ReloadWeapon;

	////////////////////////////////////////////////////////////////
	// Weapon Getters

	UFUNCTION(BlueprintCallable)
	int32 GetCurrentAmmo() { return CurrentAmmo; }

	UFUNCTION(BlueprintCallable)
	int32 GetClipSize() { return AmmoPerClip; }

	UFUNCTION(BlueprintCallable)
	float GetReloadTime() { return ReloadWeaponTime; }

	UFUNCTION(BlueprintCallable)
	FTimerHandle GetReloadTimeHandler() { return TimerHandle_ReloadWeapon; }

	UFUNCTION(BlueprintCallable)
	bool GetDefaultWeapon() { return bCanReload; }

	//
	////////////////////////////////////////////////////////////////

	virtual void StartRecharge();

	//Server Function For StartRecharge
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerStartRecharge();
	void ServerStartRecharge_Implementation();
	bool ServerStartRecharge_Validate() { return true; }

	void IncreaseAmmo(int32 Ammount);

	//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// MuzzleFlash

	UPROPERTY(ReplicatedUsing = OnRep_BurstCounter)
	int32 BurstCounter;

	UFUNCTION()
	void OnRep_BurstCounter();

	void SpawnMuzzleFlash();

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
