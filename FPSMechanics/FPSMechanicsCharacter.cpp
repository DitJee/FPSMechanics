// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPSMechanicsCharacter.h"
#include "FPSMechanicsProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "FPSMechanicsHUD.h"
#include "DrawDebugHelpers.h"
#include "Components/PrimitiveComponent.h"
#include "UObject/NameTypes.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "Particles/ParticleSystem.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/KismetMathLibrary.h"


DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AFPSMechanicsCharacter

AFPSMechanicsCharacter::AFPSMechanicsCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	// Create muzzle location scene component
	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 60.0f, 9.4f));

	// Create fire location scene component for jetpack particle
	FP_FireLocation = CreateDefaultSubobject<USceneComponent>(TEXT("FireLocation"));
	FP_FireLocation->SetupAttachment(FP_Gun);
	FP_FireLocation->SetRelativeLocation(FVector(0.2f, 120.0f, 9.4f));

	// create scene object for held object's location
	FP_HeldObjectLocation = CreateDefaultSubobject<USceneComponent>(TEXT("HeldObjectLocation"));
	FP_HeldObjectLocation->SetupAttachment(FP_MuzzleLocation);
	FP_HeldObjectLocation->SetRelativeLocation(FVector(0.0f, HoldingRange, 25.0f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Physics handle component
	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));

	// Smoke Particle System Component for jetpack
	SmokeParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SmokeParticleSystem"));
	SmokeParticleSystemComponent->SetupAttachment(Mesh1P);
	SmokeParticleSystemComponent->SetRelativeLocation(SmokeParticleLocation);

	// Smoke Particle System Component for jetpack
	HoldingParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("HoldingParticleSystem"));
	HoldingParticleSystemComponent->SetupAttachment(FP_MuzzleLocation);
	HoldingParticleSystemComponent->SetRelativeLocation(FVector(0.0f, 50.0f, 0.0f));

}

void AFPSMechanicsCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	if (bUsingMotionControllers)
	{
		Mesh1P->SetHiddenInGame(true, true);
	}
	else
	{
		Mesh1P->SetHiddenInGame(false, true);
	}

	// assign player controller
	PlayerController = GetWorld()->GetFirstPlayerController();

	// set timer for crosshair coloring
	GetWorldTimerManager().SetTimer(CrosshairHandle, this, &AFPSMechanicsCharacter::CheckIsHit, 0.01f, true);

	// deactivate smoke particle
	SmokeParticleSystemComponent->Deactivate();

	// deactivate holding particle
	HoldingParticleSystemComponent->Deactivate();
}

void AFPSMechanicsCharacter::Tick(float DeltaSeconds)
{

	Super::Tick(DeltaSeconds);

	// Update held object's and holding particle's location
	if (PhysicsHandle != nullptr && 
		FP_HeldObjectLocation != nullptr)
	{
		FVector HeldComponentLocation = FP_HeldObjectLocation->GetComponentLocation();

		PhysicsHandle->SetTargetLocation(HeldComponentLocation);
		
	}

	// refuel jetpack
	if (bIsRefuelable)
	{
		float Current = Fuel;
		float Target = FMath::Clamp(Fuel + RefuelingRate, 0.0f, MaxFuel);

		// Update fuel level
		Fuel = FMath::FInterpTo(Current, Target, DeltaSeconds, 20.0f);
	}

	// Get HUD
	auto* HUD = Cast<AFPSMechanicsHUD>(PlayerController->GetHUD());

	// set the new fuel level
	HUD->SetFuelLevel(Fuel/ MaxFuel);

}

//////////////////////////////////////////////////////////////////////////
// Input

void AFPSMechanicsCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind Dash event
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &AFPSMechanicsCharacter::OnDash);

	// Bind Pick/Drop events
	PlayerInputComponent->BindAction("Pick/Drop", IE_Pressed, this, &AFPSMechanicsCharacter::OnPickUp);
	PlayerInputComponent->BindAction("Pick/Drop", IE_Released, this, &AFPSMechanicsCharacter::OnDrop);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFPSMechanicsCharacter::OnPressedJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AFPSMechanicsCharacter::OnReleasedJump);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPSMechanicsCharacter::OnFire);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AFPSMechanicsCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPSMechanicsCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AFPSMechanicsCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AFPSMechanicsCharacter::LookUpAtRate);
}

void AFPSMechanicsCharacter::OnPressedJump()
{
	bIsRefuelable = false;

	CharacterMovement = GetCharacterMovement();

	// Start the Jetpack if possible
	if (CharacterMovement->IsFalling() && Fuel > 0.0f)
	{
		// Change gravity scale
		CharacterMovement->GravityScale = 0;

		// manipulate character's velocity
		FVector Velocity = CharacterMovement->Velocity;
		Velocity.X *= AirBrake;
		Velocity.Y *= AirBrake;

		FVector ThrustVector;
		ThrustVector.X = Velocity.X;
		ThrustVector.Y = Velocity.Y;
		ThrustVector.Z = Thrust;

		// Update Character velocity
		CharacterMovement->Velocity = ThrustVector;

		// set hovering flag
		bIsHovering = true;

		// reduce the amount of fuel
		ConsumeFuel();

		// spawn particle
		if (SmokeParticleSystemComponent != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Smoking"));
			SmokeParticleSystemComponent->Activate();
		}

		// try and play the sound if specified
		if (JetpackSound != nullptr)
		{
			JetpackSoundComponent = UGameplayStatics::SpawnSound2D(this, JetpackSound, 1.0f);
		}
	}
	else
	{
		// try and play the sound if specified
		if (JumpingSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, JumpingSound, GetActorLocation());
		}

		// Do a normal jump
		this->Jump();
	}
}

void AFPSMechanicsCharacter::ConsumeFuel()
{
	// calculate the new fuel level 
	Fuel = FMath::Clamp(Fuel - FuelConsumptionRate, 0.0f, MaxFuel);

	if (Fuel <= 0.0)
	{
		FallDown();
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("Current fuel: %f"), Fuel);
		
		// keeps reducing the fuel level
		GetWorldTimerManager().SetTimer(FuelHandle, this, &AFPSMechanicsCharacter::ConsumeFuel, GetWorld()->DeltaTimeSeconds, false);
	}
}

void AFPSMechanicsCharacter::OnReleasedJump()
{
	// stop reducing fuel level
	GetWorldTimerManager().ClearTimer(FuelHandle);

	// stop jumping
	this->StopJumping();

	// fall down
	FallDown();

	// set a delay then start refueling
	GetWorldTimerManager().SetTimer(ReFuelHandle, this, &AFPSMechanicsCharacter::SetIsRefuelable, RefuelDelay, false);
}

void AFPSMechanicsCharacter::FallDown()
{
	if (!bIsHovering) return;

	// deactivate particle
	if (SmokeParticleSystemComponent != nullptr)
	{
		SmokeParticleSystemComponent->Deactivate();
	}

	// stop playing SFX
	if (JetpackSoundComponent != nullptr)
	{
 		JetpackSoundComponent->SetActive(false);
	}

	// change gravity scale to 1
	CharacterMovement = GetCharacterMovement();

	if (CharacterMovement != nullptr)
	{
		CharacterMovement->GravityScale = 1;
	}
	
	bIsHovering = false;
}

void AFPSMechanicsCharacter::SetIsRefuelable()
{
	bIsRefuelable = true;
}

void AFPSMechanicsCharacter::OnDash()
{
	// Get Dash direction
	CurrentVelocity = GetDashDirection();

	// no dashing if we are not moving
	if (CurrentVelocity.IsZero()) return;

	// Start dashing if possible
	if (bCanDash)
	{
		// reset distance traveled to 0
		DistanceTraveled = 0.0f;

		// try and play the sound if specified
		if (DashSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, DashSound, GetActorLocation());
		}

		// start dashing
		Dash();

		// delay before we can do another dash
		GetWorldTimerManager().SetTimer(ResetDashHandle, this, &AFPSMechanicsCharacter::ResetDash, 1.0f, false);
	}
}

void AFPSMechanicsCharacter::Dash()
{

	float DeltaSeconds = GetWorld()->GetDeltaSeconds();

	// not allow the player to override the dash
	this->DisableInput(PlayerController);

	bCanDash = false;

	// calcualte dashing velocity
	FVector DashVelocity;
	DashVelocity = CurrentVelocity * DashSpeed;
	DashVelocity.Z = 0.0f;

	// update the location overtime
	this->AddActorWorldOffset(DashVelocity * DeltaSeconds, true, false);

	// calculate the new distance traveled
	DistanceTraveled += (DashVelocity * DeltaSeconds).Size();

	//UE_LOG(LogTemp, Warning, TEXT("DistanceTraveled => %f"), DistanceTraveled);

	// stop the dash once the dashing distance is covered
	if (DistanceTraveled >= DashDistance)
	{
		// enable the input
		this->EnableInput(PlayerController);

		// stop the dash loop
		GetWorldTimerManager().ClearTimer(DashHandle);
	}
	else
	{
		// start the dash loop
		GetWorldTimerManager().SetTimer(DashHandle, this, &AFPSMechanicsCharacter::Dash, DeltaSeconds, false);
	}

}

FVector AFPSMechanicsCharacter::GetDashDirection()
{
	FRotator Rotation = GetControlRotation();
	Rotation.Roll = 0.0f;
	Rotation.Pitch = 0.0f;

	FVector RightVector = UKismetMathLibrary::GetRightVector(Rotation);

	FVector ForwardVector = UKismetMathLibrary::GetForwardVector(Rotation);

	FVector DashDirectionRight;
	DashDirectionRight = RightVector * InputDirection.X;

	FVector DashDirectionForward;
	DashDirectionForward = ForwardVector * InputDirection.Y;

	FVector DashDirection = DashDirectionRight + DashDirectionForward;

	return DashDirection;
}

void AFPSMechanicsCharacter::ResetDash()
{
	// reset the dash flag
	bCanDash = !bCanDash;
	GetWorldTimerManager().ClearTimer(ResetDashHandle);
	UE_LOG(LogTemp, Warning, TEXT("resetting dash"));

}

void AFPSMechanicsCharacter::CheckIsHit()
{
	// init line trace result
	FHitResult OutHit;

	// set start vector
	FVector Start = FP_Gun->GetComponentLocation();

	// get forward vector from the FPS camera
	FVector ForwardVector = FirstPersonCameraComponent->GetForwardVector();

	// calculate the ending vector
	FVector End = (ForwardVector * PickUpRadius) + Start;

	// set the collision parameters
	static FName TraceTag = TEXT("TraceTag");
	FCollisionQueryParams Params(TraceTag);
	Params.AddIgnoredActor(this);
	
	// Get HUD
	auto* HUD = Cast<AFPSMechanicsHUD>(PlayerController->GetHUD());

	// start line tracing
	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECollisionChannel::ECC_Visibility, Params))
	{
		//DrawDebugLine(GetWorld(), Start, End, FColor::Green, true);

		if (OutHit.bBlockingHit && OutHit.GetComponent()->IsSimulatingPhysics())
		{
			//UE_LOG(LogTemp, Warning, TEXT("Found something within %f"), PickUpRadius);

			// change the crosshair color
			HUD->DrawCrosshair(true);

			// set the flah that the object can be picked
			bCanPickUp = true;

			// assign the component as pickable
			PickUpComponent = OutHit.GetComponent();
		}
		else
		{
			//UE_LOG(LogTemp, Warning, TEXT("No something within %f"), PickUpRadius);

			// change the crosshair color
			HUD->DrawCrosshair(false);

			// set the flah that the object cannot be picked
			bCanPickUp = false;
		}
	}
	else
	{
		// change the crosshair color
		HUD->DrawCrosshair(false);

		// set the flah that the object cannot be picked
		bCanPickUp = false;
	}
}

void AFPSMechanicsCharacter::OnPickUp()
{
	if (bCanPickUp && PickUpComponent != nullptr)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Picking up"));

		// get held object location
		FVector GrabLocation = PickUpComponent->GetComponentLocation();

		// get held object rotation
		FRotator GrabRotation = PickUpComponent->GetComponentRotation();

		// grab the object
		PhysicsHandle->GrabComponentAtLocationWithRotation(PickUpComponent, FName("None"), GrabLocation, GrabRotation);

		// we are holding the object now
		bIsHoldingObject = true;

		// try and play the sound if specified
		if (PickupSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
		}

		// try and play the sound if specified
		if (HoldingSound != nullptr)
		{
			HoldingSoundComponent = UGameplayStatics::SpawnSound2D(this, HoldingSound, 1.0f);
		}

		// spawn particle
		if (HoldingParticleSystemComponent != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Holding"));
			HoldingParticleSystemComponent->Activate();
		}

	}
}

void AFPSMechanicsCharacter::OnDrop()
{
	if (PhysicsHandle != nullptr)
	{
		// drop the object
		PhysicsHandle->ReleaseComponent();

		// we are not holding an object
		bIsHoldingObject = false;

		//UE_LOG(LogTemp, Warning, TEXT("Dropped Object"));

		if (HoldingSoundComponent != nullptr)
		{
			// deactivate SFX
			HoldingSoundComponent->SetActive(false);
		}

		// deactivate particle
		if (HoldingParticleSystemComponent != nullptr)
		{
			HoldingParticleSystemComponent->Deactivate();
		}
	}
	
}

void AFPSMechanicsCharacter::OnFire()
{
	if (bIsHoldingObject)
	{
		// drop the object before launching it
		OnDrop();

		// get launching direction
		FVector LaunchDirection = FirstPersonCameraComponent->GetForwardVector();

		// get launching vector
		FVector LaunchVector = LaunchDirection * LaunchForce;

		if (PickUpComponent != nullptr)
		{
			// launch the object
			PickUpComponent->AddImpulse(LaunchVector, FName("None"), true);
		}

		// spawn particle
		if (FireEmitter != nullptr)
		{
			FVector FireVector = FP_FireLocation->GetComponentLocation();

			UGameplayStatics::SpawnEmitterAtLocation(this, FireEmitter, FireVector, ParticleRotation);
		}

		// try and play the sound if specified
		if (FireSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}


		// try and play a firing animation if specified
		if (FireAnimation != nullptr)
		{
			// Get the animation object for the arms mesh
			UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
			if (AnimInstance != nullptr)
			{
				AnimInstance->Montage_Play(FireAnimation, 1.f);
			}
		}
	}
	else
	{
		// try and play the sound if specified
		if (EmptySound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, EmptySound, GetActorLocation());
		}
	}

}

void AFPSMechanicsCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}

	InputDirection.Y = Value;
}

void AFPSMechanicsCharacter::MoveRight(float Value)
{

	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}

	InputDirection.X = Value;
}

void AFPSMechanicsCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AFPSMechanicsCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}




