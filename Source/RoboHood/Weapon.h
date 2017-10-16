// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	Idle,
	Firing,
	OverHeating
};

UCLASS()
class ROBOHOOD_API AWeapon : public AActor
{
	GENERATED_BODY()

	//Projectile
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<class AFireBall> FireBall;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float TimeBetweenShots;


public:	

	// Sets default values for this actor's properties
	AWeapon();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//------------------------------------------------------------------

	void StartFire();

	void StopFire();

	EWeaponState GetCurrentState() const;

	/** set the weapon's owning pawn */
	void SetOwningPawn(class ATestCharacter* NewOwner);

	FTimerHandle HandleFiringTimerHandle;

	bool bIsEquipped;

	void OnEquip();

	void RemoveWeapon();

protected:

	///** pawn owner */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_MyPawn)
	class ATestCharacter* MyPawn;

	/** weapon mesh: 3rd person view */
	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh")
	UStaticMeshComponent* Mesh;

	UFUNCTION()
	void OnRep_MyPawn();

	/* You can assign default values to function parameters, these are then optional to specify/override when calling the function. */
	void AttachMeshToPawn();

	bool CanFire() const;

	FHitResult WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo) const;

private:

	float LastFireTime;

	void OnBurstStarted();

	void OnBurstFinished();

	EWeaponState CurrentState;

	bool bRefiring;

	bool bWantsToFire;

	void SetWeaponState(EWeaponState NewState);

	void DetermineWeaponState();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerStartFire();
	void ServerStartFire_Implementation();
	bool ServerStartFire_Validate();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerStopFire();
	void ServerStopFire_Implementation();
	bool ServerStopFire_Validate();

	virtual void HandleFiring();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerHandleFiring();
	void ServerHandleFiring_Implementation();
	bool ServerHandleFiring_Validate();

	void FireWeapon();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerFireWeapon();
	void ServerFireWeapon_Implementation();
	bool ServerFireWeapon_Validate();
	
};
