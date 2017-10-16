// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GameFramework/Character.h"
#include "TestCharacter.generated.h"

UCLASS()
class ROBOHOOD_API ATestCharacter : public ACharacter
{
	GENERATED_BODY()

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	//CameraBoom
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	//Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	//Projectile
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<class AFireBall> FireBall;

	//Particle Effect
	UPROPERTY(EditDefaultsOnly, Category = "Particles", Meta = (BlueprintProtected = "true"))
	UParticleSystem* Particle;

	//Attach Point For The Weapon
	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
	FName WeaponAttachPoint;

	//The Players Weapon
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<class AWeapon> DefaultWeapon;

public:

	//Basic Constructor
	ATestCharacter();

public:

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

protected:

	//Movement Functions
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

public:	

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PostInitializeComponents();

public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

public:

	//This Is The Variable That I Replicate
	UPROPERTY(Replicated)
	float Health;

	bool bWantsToFire;

	/* Mapped to input */
	void OnStartFire();

	/* Mapped to input */
	void OnStopFire();

	void StartWeaponFire();

	void StopWeaponFire();





	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

	void EquipWeapon(AWeapon* TheWeapon);

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerEquipWeapon(AWeapon* TheWeapon);
	void ServerEquipWeapon_Implementation(AWeapon* TheWeapon);
	bool ServerEquipWeapon_Validate(AWeapon* TheWeapon);


	void SpawnDefaultWeapon();

	/* OnRep functions can use a parameter to hold the previous value of the variable. Very useful when you need to handle UnEquip etc. */
	UFUNCTION()
	void OnRep_CurrentWeapon();

	void SetCurrentWeapon(class AWeapon* NewWeapon);

	UPROPERTY(Transient, ReplicatedUsing = OnRep_CurrentWeapon)
	class AWeapon* CurrentWeapon;



	FName GetWeaponAttachPoint();

	
};
