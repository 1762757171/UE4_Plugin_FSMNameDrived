#include "GraphEditor/SchemaActions/FSMSchemaAction_NewNode.h"
#include "UnrealEd.h"
#include "UnrealMath.h"
#include "ScopedTransaction.h"
#include "FSM.h"
#include "GraphEditor/FSMGraph.h"
#include "GraphEditor/Nodes/FSMGraphNode.h"

#define LOCTEXT_NAMESPACE "FSMAssetEditorNativeNames"

UEdGraphNode * FFSMSchemaAction_NewNode::PerformAction(UEdGraph * ParentGraph, UEdGraphPin * FromPin, const FVector2D Location, bool bSelectNewNode)
{
	const FScopedTransaction Transaction(LOCTEXT("EditorNewNode", "FSM Editor: New Node"));
	UFSM* FSM = CastChecked<UFSMGraph>(ParentGraph)->GetFSM();

	FString NodeNameStr = FString("NewNode_");
	NodeNameStr.AppendInt(FMath::RandRange(0, 1000));		//Temporary
	FName NewStateNode(*NodeNameStr);
	FSM->AddState(NewStateNode);

	verify(FSM->Modify());

	// Mark for modification
	verify(ParentGraph->Modify());
	if (FromPin)
	{
		verify(FromPin->Modify());
	}


	// Maximum distance a drag can be off a node edge to require 'push off' from node
	//constexpr int32 NodeDistance = 60;

	// Create the graph node
	FGraphNodeCreator<UFSMGraphNode> NodeCreator(*ParentGraph);
	UFSMGraphNode* GraphNode = NodeCreator.CreateUserInvokedNode(bSelectNewNode);

	GraphNode->SetFlags(RF_Transactional);
	GraphNode->Rename(*(FString("GraphNode_") + NodeNameStr), ParentGraph);
	GraphNode->SetName(NewStateNode);

	// Finalize graph node creation
	NodeCreator.Finalize(); // Calls on the node: CreateNewGuid, PostPlacedNewNode, AllocateDefaultPins
	GraphNode->AutowireNewNode(FromPin);


	GraphNode->NodePosX = Location.X;
	GraphNode->NodePosY = Location.Y;
	//ResultNode->SnapToGrid(SNAP_GRID);
	//

	FSM->PostEditChange();
	FSM->MarkPackageDirty();

	ParentGraph->NotifyGraphChanged();

	return GraphNode;
}
#undef LOCTEXT_NAMESPACE