// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FPSMECHFuelTank.generated.h"

class UProgressBar;
/**
 * 
 */
UCLASS()
class FPSMECHANICS_API UFPSMECHFuelTank : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta = (BindWidget))
		UProgressBar* FuelTank;

	void SetFuelLevel(float fuelLevel);
	
};
