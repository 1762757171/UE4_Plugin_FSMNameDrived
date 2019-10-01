// Fill out your copyright notice in the Description page of Project Settings.

#include "FSMAssetTypeAction.h"
#include "FSM.h"
#include "AssetTypeCategories.h"
#include "FSMAssetEditor.h"

UClass * FAssetTypeAction_FSM::GetSupportedClass() const
{
	return UFSM::StaticClass();
}

void FAssetTypeAction_FSM::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto Object : InObjects)
	{
		auto FSM = Cast<UFSM>(Object);
		if (FSM != nullptr)
		{
			TSharedRef<FFSMAssetEditor> Editor(new FFSMAssetEditor);
			Editor->InitializeAssetEditor(Mode, EditWithinLevelEditor, FSM);
		}
	}
}

uint32 FAssetTypeAction_FSM::GetCategories()
{
	return EAssetTypeCategories::Gameplay;
}
