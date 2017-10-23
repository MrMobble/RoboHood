// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
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

	//Multiplayer Framework For Replicating Variables
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	//Socket Name Weapon Attaches To.
	UPROPERTY(EditDefaultsOnly, Category = "WeaponInfo")
	FName WeaponAttachPoint;

	//The Players Default Weapon.
	UPROPERTY(EditDefaultsOnly, Category = "WeaponInfo")
	TSubclassOf<class ARWeapon> DefaultWeapon;

	//Called To Bind Functionality To Input.
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Allow Actors To Initialize Themselves.
	virtual void PostInitializeComponents();

	//Returns CameraBoom
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	//Returns FollowCamera
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	//Apply Damage To The Players Health
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

	//Base Turn Rate
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	//Base Look Up/Down Rate
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	//Player Health Replicated
	UPROPERTY(Replicated)
	float Health;

	UFUNCTION(BlueprintCallable)
	float GetHealth() { return Health; }

private:

	//CameraBoom
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	//Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

protected:

	//Movement Functions
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Create And Equip Weapon Functions And Variables
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:

	//Returns The Weapon Attack Point
	FName GetWeaponAttachPoint();

	void SpawnDefaultWeapon();

	void EquipWeapon(ARWeapon* TheWeapon);

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerEquipWeapon(ARWeapon* TheWeapon);
	void ServerEquipWeapon_Implementation(ARWeapon* TheWeapon);
	bool ServerEquipWeapon_Validate(ARWeapon* TheWeapon);

	UPROPERTY(Transient, ReplicatedUsing = OnRep_CurrentWeapon)
	class ARWeapon* CurrentWeapon;

	//This Is Added In The Case That We Decide That You Can Pick Up A Special Weapon (Prob Not)
	UFUNCTION()
	void OnRep_CurrentWeapon();

	void SetCurrentWeapon(class ARWeapon* NewWeapon);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	ARWeapon* GetCurrentWeapon();

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

	void ReloadWeapon();
	
};
