// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "FPSMechanicsHUD.generated.h"

class UFPSMECHCrossHair;
class UUserWidget;
class UFPSMECHFuelTank;

UCLASS()
class AFPSMechanicsHUD : public AHUD
{
	GENERATED_BODY()

public:
	AFPSMechanicsHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

	virtual void BeginPlay() override;

	void DrawCrosshair(bool bFlag);

	void SetFuelLevel(float fuelLevel);

	UPROPERTY(EditAnywhere)
		TSubclassOf<UFPSMECHCrossHair> WBP_Crosshair;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UFPSMECHFuelTank> WBP_FuelTank;
private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

	UUserWidget* W_Crosshair;

	UUserWidget* W_FuelTank;

};

