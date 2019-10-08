#include "GraphEditor/FSMGraphFactories.h"
#include "GraphEditor/FSMGraphSchema.h"
#include "GraphEditor/Nodes/FSMGraphNode.h"
#include "GraphEditor/Nodes/FSMGraphEntryNode.h"
#include "GraphEditor/Pins/FSMGraphPin.h"
#include "GraphEditor/Connection/FSMConnectionDrawingPolicy.h"
#include "GraphEditor/Connection/FSMTransitionNode.h"
#include "SGraphPinExec.h"

#define LOCTEXT_NAMESPACE "FSMAssetEditorNativeNames"

TSharedPtr<class SGraphNode> FFSMGraphNodeFactory::CreateNode(class UEdGraphNode* InNode) const
{
	if (auto Node = Cast<UFSMGraphNode>(InNode))
	{
		return SNew(SFSMGraphNode, Node);
	}
	if (auto Node = Cast<UFSMTransitionNode>(InNode))
	{
		return SNew(SFSMTransitionNode, Node);
	}
	if (auto Node = Cast<UFSMGraphEntryNode>(InNode))
	{
		return SNew(SFSMGraphEntryNode, Node);
	}
	return nullptr;
}



TSharedPtr<class SGraphPin> FFSMGraphPinFactory::CreatePin(UEdGraphPin * Pin) const
{
	if (Pin->PinType.PinCategory == UFSMGraphSchema::PC_Trans)
	{
		return SNew(SFSMGraphPin, Pin);
	}
	if (Pin->PinType.PinCategory == UFSMGraphSchema::PC_Entry)
	{
		return SNew(SGraphPinExec, Pin);
	}

	return nullptr;
}

FConnectionDrawingPolicy * FFSMGraphPinConnectionFactory::CreateConnectionPolicy(const UEdGraphSchema * Schema, int32 InBackLayerID, int32 InFrontLayerID, float ZoomFactor, const FSlateRect & InClippingRect, FSlateWindowElementList & InDrawElements, UEdGraph * InGraphObj) const
{
	if (Schema->IsA(UFSMGraphSchema::StaticClass())) {
		return new FFSMConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, ZoomFactor, InClippingRect, InDrawElements, InGraphObj);
	}
	return nullptr;
}

#undef LOCTEXT_NAMESPACE