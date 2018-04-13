// Property Of Me

#pragma once

#include "GameFramework/Pawn.h"
#include "RProjectileBase.h"
#include "RBot.generated.h"

UCLASS()
class ROBOHOOD_API ARBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	FRotator NewForwardDirection;

	UFloatingPawnMovement* Movement;

	/** Sphere Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* CollisionSphereComponent;

	//Bot Projectile
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ARProjectileBase> BotProjectile;

	UPROPERTY(EditAnywhere)
	float timeBetweenShots = 5.0f;

	FTimerHandle timerHandle;

	UFUNCTION(NetMulticast, Reliable)
	void Shoot();
	void Shoot_Implementation();

	//OnHit Function
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
};
