// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSMECHCrossHair.h"
#include "Components/Image.h"

void UFPSMECHCrossHair::NativeConstruct()
{
	Super::NativeConstruct();

	bIsHit = false;
}