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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Basic Weapon Functions And Variables
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
public:	

	//Deafult Constructor.
	ARWeapon();

	//Projectile.
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	TSubclassOf<class ARProjectileBase> Projectile;

	//FireRate.
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	float TimeBetweenShots;

	//Called From Player Sets MyPawn
	void SetOwningPawn(class ARCharacter* NewOwner);

	//Attaches Weapon To Player And Sets Equipped To True.
	void OnEquip();

	//Destroys The Weapno When Player Dies
	void RemoveWeapon();

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

	//Attaches Weapon To Player Socket
	void AttachMeshToPawn();

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

	//Spawns The Projectile (Needs Work)
	void FireWeapon();

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

	//Weapons Current Ammo Replicated
	UPROPERTY(Transient, Replicated)
	int32 CurrentAmmo;

	//MaxHeat
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	int32 MaxAmmo;

	//ReloadTime
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Info")
	float ReloadWeaponTime;

	UFUNCTION(BlueprintCallable)
	int32 GetCurrentAmmo() { return CurrentAmmo; }

	UFUNCTION(BlueprintCallable)
	int32 GetMaxAmmo() { return MaxAmmo; }

	UFUNCTION(BlueprintCallable)
	bool GetIsReloading() { return bIsReloading; }

protected:

	void UseAmmo();

	void StopRecharge();

	void RechargeWeapon();

	bool bIsReloading;
	
private:

	FTimerHandle TimerHandle_ReloadWeapon;


	
};
