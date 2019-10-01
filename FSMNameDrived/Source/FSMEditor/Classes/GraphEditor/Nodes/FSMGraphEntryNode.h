// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "EdGraph/EdGraphNode.h"
#include "SNodePanel.h"
#include "SGraphNode.h"
#include "FSMGraphEntryNode.generated.h"

UCLASS(MinimalAPI)
class UFSMGraphEntryNode : public UEdGraphNode
{
	GENERATED_BODY()

public:
		//~ Begin UEdGraphNode Interface
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override; 
	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;
	//~ End UEdGraphNode Interface

	FSMEDITOR_API UEdGraphNode* GetOutputNode() const;

};

class SFSMGraphEntryNode : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SFSMGraphEntryNode) {}
	SLATE_END_ARGS()
public:

	void Construct(const FArguments& InArgs, UFSMGraphEntryNode* InNode);

	// SNodePanel::SNode interface
	virtual void GetNodeInfoPopups(FNodeInfoContext* Context, TArray<FGraphInformationPopupInfo>& Popups) const override;
	// End of SNodePanel::SNode interface

	// SGraphNode interface
	virtual void UpdateGraphNode() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;

	// End of SGraphNode interface


protected:
	FSlateColor GetBorderBackgroundColor() const;

	FText GetPreviewCornerText() const;
};
