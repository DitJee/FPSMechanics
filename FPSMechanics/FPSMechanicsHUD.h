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

	virtual void BeginPlay() override;

	void DrawCrosshair(bool bFlag);

	void SetFuelLevel(float fuelLevel);

	UPROPERTY(EditAnywhere)
		TSubclassOf<UFPSMECHCrossHair> WBP_Crosshair;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UFPSMECHFuelTank> WBP_FuelTank;
private:

	UUserWidget* W_Crosshair;

	UUserWidget* W_FuelTank;

};

