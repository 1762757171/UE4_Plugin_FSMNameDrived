// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "FSMEditor.h"
#include "FSMAssetTypeAction.h"
#include "GraphEditor/FSMGraphFactories.h"

#define LOCTEXT_NAMESPACE "FSMAssetEditorNativeNames"

void FFSMEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	IAssetTools& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	TSharedPtr<FAssetTypeAction_FSM> FSMAssetTypeAction = MakeShareable(new FAssetTypeAction_FSM);
	AssetToolsModule.RegisterAssetTypeActions(FSMAssetTypeAction.ToSharedRef());

	// Create factories
	GraphNodeFactory = MakeShared<FFSMGraphNodeFactory>();
	FEdGraphUtilities::RegisterVisualNodeFactory(GraphNodeFactory);
	GraphPinFactory = MakeShared<FFSMGraphPinFactory>();
	FEdGraphUtilities::RegisterVisualPinFactory(GraphPinFactory);
	GraphConnectionFactory = MakeShared<FFSMGraphPinConnectionFactory>();
	FEdGraphUtilities::RegisterVisualPinConnectionFactory(GraphConnectionFactory);
}

void FFSMEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FEdGraphUtilities::UnregisterVisualNodeFactory(GraphNodeFactory);
	FEdGraphUtilities::UnregisterVisualPinFactory(GraphPinFactory);
	FEdGraphUtilities::UnregisterVisualPinConnectionFactory(GraphConnectionFactory);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FFSMEditorModule, FSMEditor)