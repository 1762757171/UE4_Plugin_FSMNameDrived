// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "GraphEditor/Nodes/FSMGraphEntryNode.h"
#include "EdGraph/EdGraph.h"
#include "GraphEditor/FSMGraphSchema.h"
#include "Widgets/SBoxPanel.h"
#include "SGraphPin.h"
#include "GraphEditor/FSMGraph.h"
#include "GraphEditor/Nodes/FSMGraphNode.h"
#include "FSM.h"

#define LOCTEXT_NAMESPACE "FSMAssetEditorNativeNames"

/////////////////////////////////////////////////////
// UFSMGraphEntryNode

void UFSMGraphEntryNode::AllocateDefaultPins()
{
	UEdGraphPin* Outputs = CreatePin(EGPD_Output, UFSMGraphSchema::PC_Entry, TEXT("Entry"));
}

FText UFSMGraphEntryNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	UEdGraph* Graph = GetGraph();
	return FText::FromString(Graph->GetName());
}

FText UFSMGraphEntryNode::GetTooltipText() const
{
	return LOCTEXT("StateEntryNodeTooltip", "Entry point for FSM");
}

void UFSMGraphEntryNode::PinConnectionListChanged(UEdGraphPin * Pin)
{
	auto TargetNode = Cast<UFSMGraphNode>(GetOutputNode());
	auto Graph = Cast<UFSMGraph>(GetGraph());
	auto FSM = Graph->GetFSM();
	FSM->SetEntryState(TargetNode->GetNodeName());
}

UEdGraphNode* UFSMGraphEntryNode::GetOutputNode() const
{
	if (Pins.Num() > 0 && Pins[0] != NULL)
	{
		check(Pins[0]->LinkedTo.Num() <= 1);
		if (Pins[0]->LinkedTo.Num() > 0 && Pins[0]->LinkedTo[0]->GetOwningNode() != NULL)
		{
			return Pins[0]->LinkedTo[0]->GetOwningNode();
		}
	}
	return NULL;
}

void SFSMGraphEntryNode::Construct(const FArguments& InArgs, UFSMGraphEntryNode* InNode)
{
	this->GraphNode = InNode;

	this->SetCursor(EMouseCursor::CardinalCross);

	this->UpdateGraphNode();
}

void SFSMGraphEntryNode::GetNodeInfoPopups(FNodeInfoContext* Context, TArray<FGraphInformationPopupInfo>& Popups) const
{

}

FSlateColor SFSMGraphEntryNode::GetBorderBackgroundColor() const
{
	FLinearColor InactiveStateColor(0.08f, 0.08f, 0.08f);
	FLinearColor ActiveStateColorDim(0.4f, 0.3f, 0.15f);
	FLinearColor ActiveStateColorBright(1.f, 0.6f, 0.35f);

	return InactiveStateColor;
}

void SFSMGraphEntryNode::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();

	// Reset variables that are going to be exposed, in case we are refreshing an already setup node.
	RightNodeBox.Reset();
	LeftNodeBox.Reset();


	FLinearColor TitleShadowColor(0.6f, 0.6f, 0.6f);

	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
	this->GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("Graph.StateNode.Body"))
			.Padding(0)
			.BorderBackgroundColor(this, &SFSMGraphEntryNode::GetBorderBackgroundColor)
			[
				SNew(SOverlay)

				// PIN AREA
				+ SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.Padding(10.0f)
				[
					SAssignNew(RightNodeBox, SVerticalBox)
				]
			]
		];

	CreatePinWidgets();
}

void SFSMGraphEntryNode::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	PinToAdd->SetOwner(SharedThis(this));
	RightNodeBox->AddSlot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.FillHeight(1.0f)
		[
			PinToAdd
		];
	OutputPins.Add(PinToAdd);
}

FText SFSMGraphEntryNode::GetPreviewCornerText() const
{
	return LOCTEXT("CornerTextDescription", "Entry point for FSM");
}

#undef LOCTEXT_NAMESPACE
