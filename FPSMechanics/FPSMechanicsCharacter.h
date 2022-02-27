// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSMechanicsCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UMotionControllerComponent;
class UAnimMontage;
class USoundBase;
class UCharacterMovementComponent;
class APlayerController;
class UPhysicsHandleComponent;
class UPrimitiveComponent;
class USoundCue;
class UAudioComponent;
class UParticleSystem;
class UParticleSystemComponent;

UCLASS(config=Game)
class AFPSMechanicsCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* FP_MuzzleLocation;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USceneComponent* FP_FireLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
		UPhysicsHandleComponent* PhysicsHandle;

	/** Location in front of the muzzle for the object */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USceneComponent* FP_HeldObjectLocation;

	/** Booster particle for jetpack */
	UPROPERTY(EditDefaultsOnly, Category = Mesh)
	UParticleSystemComponent* SmokeParticleSystemComponent;

	/** Booster particle for jetpack */
	UPROPERTY(EditDefaultsOnly, Category = Mesh)
		UParticleSystemComponent* HoldingParticleSystemComponent;

public:
	AFPSMechanicsCharacter();

protected:
	virtual void BeginPlay();

	virtual void Tick(float DeltaSeconds) override;

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AFPSMechanicsProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SFX")
	USoundBase* FireSound;

	/** Sound to play each time we dash */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX")
		USoundBase* DashSound;

	/** Sound to play each time we pick an object */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX")
		USoundBase* PickupSound;

	/** Sound to play each time we jump */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX")
		USoundBase* JumpingSound;

	/** Sound to play each time we hold an object */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX")
	USoundCue* HoldingSound;

	/** Sound to play each time we are jetpacking */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX")
		USoundCue* JetpackSound;

	/** Sound to play each time we did not hold any object */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX")
		USoundBase* EmptySound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	/** particle that plays when we are jetpacking*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles")
	UParticleSystem* FireEmitter;

	/** particle rotaion*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles")
		FRotator ParticleRotation;

	/** jetpack smoke vector*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles")
		FVector SmokeParticleLocation;

	/** Whether to use motion controller location for aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	uint8 bUsingMotionControllers : 1;

	/** Dash Speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay_Dash)
		float DashSpeed; 

	/** Dash Distance */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay_Dash)
		float DashDistance;

	/** distance between held object and muzzle*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay_GravGun)
		float HoldingRange = 300.0f;

	/** distance allowed to pickup an object*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay_GravGun)
		float PickUpRadius;

	/** Impulse with fire an object*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay_GravGun)
		float LaunchForce = 500.0f;

	/** Maximum fuel allowed*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay_JetPack)
		float MaxFuel = 100.0f;

	/** Starting fuel*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay_JetPack)
		float Fuel = 100.0f;

	/** Jetpack thrust*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay_JetPack)
		float Thrust = 100.0f;

	/** the decrease in movement velocity in percent*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay_JetPack)
		float AirBrake = 0.75f;

	/** The rate of which the fuel is refueled*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay_JetPack)
		float RefuelingRate = 10.f;

	/** The rate of fuel consumption*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay_JetPack)
		float FuelConsumptionRate = 1.f;

	/** The time in which the fuel will be refueled*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay_JetPack)
		float RefuelDelay = 2.f;

private:

	/** Sound Components*/
	UAudioComponent* HoldingSoundComponent;
	UAudioComponent* JetpackSoundComponent;

	/** The object that is picked*/
	UPrimitiveComponent* PickUpComponent;

	/** Player Controller*/
	APlayerController* PlayerController;

	/** Character Movement Component*/
	UCharacterMovementComponent* CharacterMovement;

	/** Character's Current Velocity*/
	FVector CurrentVelocity;

	/** Direction vector from input*/
	FVector InputDirection = FVector::ZeroVector;

	/** Distance Traveled*/
	float DistanceTraveled = 0.0f;

	bool bCanDash = true;
	bool bCanPickUp = false;
	bool bIsHoldingObject = false;
	bool bIsHovering = false;
	bool bIsRefuelable = false;

	/** Timers */
	FTimerHandle  CrosshairHandle;
	FTimerHandle  ResetDashHandle;
	FTimerHandle  DashHandle;
	FTimerHandle  FuelHandle;
	FTimerHandle  ReFuelHandle;

protected:

	/** set refuelability*/
	void SetIsRefuelable();

	/** disable jetpack*/
	void FallDown();

	/** reduce the amount of fuel overtime*/
	void ConsumeFuel();

	/** jump or enable jetpack*/
	void OnPressedJump();

	/** cancel jump or disable jetpack*/
	void OnReleasedJump();

	/** Pick up event*/
	void OnPickUp();

	/** Drop object event*/
	void OnDrop();

	/** Check if the line trace hit pickable object*/
	void CheckIsHit();

	/** determine dashing direction*/
	FVector GetDashDirection();

	/** Dash event*/
	void OnDash();

	/** Dash loop*/
	void Dash();

	/** Reset Dash*/
	void ResetDash();
	
	/** Fires a projectile. */
	void OnFire();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};

