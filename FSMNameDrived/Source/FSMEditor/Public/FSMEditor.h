// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FFSMEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	TSharedPtr<struct FFSMGraphNodeFactory> GraphNodeFactory;
	TSharedPtr<struct FFSMGraphPinFactory> GraphPinFactory;
	TSharedPtr<struct FFSMGraphPinConnectionFactory> GraphConnectionFactory;
};
