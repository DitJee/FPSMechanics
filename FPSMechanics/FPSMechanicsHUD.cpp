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
	// Set the crosshair texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("/Game/FirstPerson/Textures/FirstPersonCrosshair"));
	CrosshairTex = CrosshairTexObj.Object;
}


void AFPSMechanicsHUD::DrawHUD()
{
	Super::DrawHUD();

	// Draw very simple crosshair

	// find center of the Canvas
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	const FVector2D CrosshairDrawPosition( (Center.X),
										   (Center.Y + 20.0f));

	// draw the crosshair
	FCanvasTileItem TileItem( CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	//Canvas->DrawItem( TileItem );

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