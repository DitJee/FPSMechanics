// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FPSMECHCrossHair.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class FPSMECHANICS_API UFPSMECHCrossHair : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsHit;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UImage* IconImage;
};
