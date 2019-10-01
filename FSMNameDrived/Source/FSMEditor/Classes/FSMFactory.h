// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "FSMFactory.generated.h"

/**
 * 
 */
UCLASS()
class FSMEDITOR_API UFSMFactory : public UFactory
{
	GENERATED_BODY()
	
public:
	UFSMFactory();

	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};
