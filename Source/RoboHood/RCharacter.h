// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "HitInfo.h"
#include "GameFramework/Character.h"
#include "RCharacter.generated.h"


UCLASS()
class ROBOHOOD_API ARCharacter : public ACharacter
{
	GENERATED_BODY()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Basic Character Functions And Movement Variables
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	//Class Constructor.
	ARCharacter();

	virtual void Tick(float DeltaTime) override;

	//Multiplayer Framework For Replicating Variables
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	//Called To Bind Functionality To Input.
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Allow Actors To Initialize Themselves.
	virtual void PostInitializeComponents();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Character Damage And Death Funtions / Variables
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	UAnimMontage* DeathAnim;

	//Apply Damage To The Players Health
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

	bool CanDie();

	bool Die(class AController* Killer, class AActor* DamageCauser);

	void OnDeath(class AController* Killer, class AActor* DamageCauser);

	void SetRagDoll();

	void StopAnimation();

	void ReplicateHit(class AController* Killer, class AActor* DamageCauser, bool bKilled);

	virtual float PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None) override;

	//Replicate where this pawn was last hit and damaged
	UPROPERTY(Transient, ReplicatedUsing = OnRep_LastTakeHitInfo)
	struct FTakeHitInfo LastTakeHitInfo;

	UFUNCTION()
	void OnRep_LastTakeHitInfo();

////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Base Turn Rate
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	//Base Look Up/Down Rate
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	UPROPERTY(EditDefaultsOnly, Category = Camera)
	float CameraDistance;

	UPROPERTY(EditDefaultsOnly, Category = Camera)
	FVector CameraOffset;

	//Player Health Replicated
	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere)
	float Health;

	//Identifies if pawn is in its dying state
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health)
	uint32 bIsDying : 1;

	//Player Health Replicated
	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere)
	bool bIsDead;

	//Identifies if pawn is in its dying state
	UPROPERTY(EditDefaultsOnly, Category = Health)
	float Base_Health;

	UFUNCTION(BlueprintCallable)
	float GetHealth() { return Health; }

	bool IsAlive();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Player Components & Movement Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	//Returns CameraBoom
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE class UCameraComponent* GetCamera() const { return TPPCamera; }

private:

	//CameraBoom
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TPPCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FPPCamera;

protected:

	//Movement Functions
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

public:

	UPROPERTY(Replicated, BlueprintReadOnly)
	float AimPitch;

	void SetPitch();

	UFUNCTION(BlueprintCallable)
	float GetAimPitch() { return AimPitch; }

	UPROPERTY(Replicated)
	bool bWantsToRun;

	bool bShiftPressed;

	float fDirection;

	UFUNCTION(BlueprintCallable)
	bool IsRunning();

	void StartRunning();
	void StopRunning();

	void SetRunning(bool bNewValue);

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerSetSprint(bool bNewValue);
	void ServerSetSprint_Implementation(bool bNewValue);
	bool ServerSetSprint_Validate(bool bNewValue) { return true; }

public:

	UPROPERTY(EditDefaultsOnly, Category = "Movement Settings")
	float Walk_Speed;

	UPROPERTY(EditDefaultsOnly, Category = "Movement Settings")
	float Run_Speed;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Create And Equip Weapon Functions And Variables
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	void SpawnWeapons();

	//Socket Name Weapon Attaches To.
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Settings")
	FName WeaponAttachPoint;

	//Array Of Player Weapons
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Settings")
	TArray<TSubclassOf<class ARWeapon>> Weapons;

	//Returns The Weapon Attack Point
	FName GetWeaponAttachPoint();

	void AddWeapon(ARWeapon* NewWeapon);

	void EquipWeapon(ARWeapon* TheWeapon);

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerEquipWeapon(ARWeapon* TheWeapon);
	void ServerEquipWeapon_Implementation(ARWeapon* TheWeapon);
	bool ServerEquipWeapon_Validate(ARWeapon* TheWeapon);

	UPROPERTY(Transient, Replicated, BlueprintReadOnly)
	TArray<ARWeapon*> Inventory;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_CurrentWeapon)
	class ARWeapon* CurrentWeapon;

	class ARWeapon* PreviousWeapon;

	int32 CurrentWeaponIndex;

	UFUNCTION(BlueprintCallable)
	int32 GetCurrentWeaponIndex() { return CurrentWeaponIndex; }

	//This Is Added In The Case That We Decide That You Can Pick Up A Special Weapon (Prob Not)
	UFUNCTION()
	void OnRep_CurrentWeapon(ARWeapon* LastWeapon);

	void SetCurrentWeapon(class ARWeapon* NewWeapon, class ARWeapon* LastWeapon = nullptr);

	UFUNCTION(BlueprintCallable)
	class ARWeapon* GetCurrentWeapon() { return CurrentWeapon; }

	void OneAction();
	void TwoAction();
	void ThreeAction();

	void SwapToNewWeaponMesh();

	void DestroyInventory();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Shoot/Reload Functions And Variables
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
	
	//Does The Player Want To Fire
	bool bWantsToFire;

	//Mapped To InputKey Pressed
	void OnStartFire();

	//Mapped To InputKey Released
	void OnStopFire();

	//Tells The Weapon To Fire
	void StartWeaponFire();

	//Tells The Weapon To StopFire
	void StopWeaponFire();

	//Reloads The Weapon
	void ReloadWeapon();

	UFUNCTION(BlueprintCallable)
	void AddAmmo(int32 AmmoIndex, int32 Ammount);

	UFUNCTION(BlueprintCallable)
	void AddHealth(int32 Ammount);
	
};
