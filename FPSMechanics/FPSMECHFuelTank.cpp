// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSMECHFuelTank.h"
#include <Components/ProgressBar.h>

void UFPSMECHFuelTank::NativeConstruct()
{
	Super::NativeConstruct();

	FuelTank->SetPercent(1.0f);
}

void UFPSMECHFuelTank::SetFuelLevel(float fuelLevel)
{
	FuelTank->SetPercent(fuelLevel);
}
