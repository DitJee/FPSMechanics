// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPSMechanicsHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "FPSMECHCrossHair.h"
#include "FPSMECHFuelTank.h"

AFPSMechanicsHUD::AFPSMechanicsHUD()
{
}

void AFPSMechanicsHUD::BeginPlay()
{
	Super::BeginPlay();

	if (WBP_Crosshair)
	{
		
		W_Crosshair = CreateWidget(GetWorld(), WBP_Crosshair);

		if (W_Crosshair)
		{
			W_Crosshair->AddToViewport();
		}
		
	}

	if (WBP_FuelTank)
	{

		W_FuelTank = CreateWidget(GetWorld(), WBP_FuelTank);

		if (W_FuelTank)
		{
			W_FuelTank->AddToViewport();
		}

	}

}

void AFPSMechanicsHUD::DrawCrosshair(bool bFlag)
{

	auto* Crosshair = Cast<UFPSMECHCrossHair>(W_Crosshair);

	Crosshair->bIsHit = bFlag;

	return;
}

void AFPSMechanicsHUD::SetFuelLevel(float fuelLevel)
{
	if (W_FuelTank)
	{
		auto* FuelTank = Cast<UFPSMECHFuelTank>(W_FuelTank);

		FuelTank->SetFuelLevel(fuelLevel);
	}
	
}