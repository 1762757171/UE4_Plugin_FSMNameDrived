// Fill out your copyright notice in the Description page of Project Settings.

#include "FSMFactory.h"
#include "FSM.h"

UFSMFactory::UFSMFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UFSM::StaticClass();
}

UObject * UFSMFactory::FactoryCreateNew(UClass * Class, UObject * InParent, FName Name, EObjectFlags Flags, UObject * Context, FFeedbackContext * Warn)
{
	auto Asset = NewObject<UFSM>(InParent, Class, Name, Flags | RF_Transactional);
	return Asset;
}