// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "TestCharacter.generated.h"

UCLASS()
class ROBOHOOD_API ATestCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AFireBall> FireBall;

public:

	//Particle system variable
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Particles", Meta = (BlueprintProtected = "true"))
	class UParticleSystemComponent* ParticleSystem;
	UParticleSystem* ExplodeParticleSystem;

public:

	// Sets default values for this character's properties
	ATestCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	void TurnAtRate(float Rate);

	void LookUpAtRate(float Rate);

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

public:

	//This Is The Variable That I Replicate
	UPROPERTY(ReplicatedUsing = OnRep_Explode)
	bool Explode;

	//This Is The Variable That I Replicate
	UPROPERTY(Replicated)
	float Health;

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);


	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

	//Self Explode Stuff
	void ExplodeON();
	void ExplodeOFF();

	void PlayerExplode();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerExplode();
	void ServerExplode_Implementation();
	bool ServerExplode_Validate();

	UFUNCTION()
	void OnRep_Explode();


	//FireBall Stuff
	void SpawnFireBall();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerSpawnFireBall();
	void ServerSpawnFireBall_Implementation();
	bool ServerSpawnFireBall_Validate();

	FHitResult WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo) const;



private:

	
};
