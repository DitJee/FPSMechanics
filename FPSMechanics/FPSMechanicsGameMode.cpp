// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPSMechanicsGameMode.h"
#include "FPSMechanicsHUD.h"
#include "FPSMechanicsCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFPSMechanicsGameMode::AFPSMechanicsGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFPSMechanicsHUD::StaticClass();
}
