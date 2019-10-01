#include "FSMGraphSchema.h"
#include "SchemaActions/FSMSchemaAction_NewNode.h"
#include "SchemaActions/FSMSchemaAction_NewComment.h"
#include "BlueprintEditorUtils.h"
#include "GraphEditor/Nodes/FSMGraphNode.h"
#include "GraphEditor/Connection/FSMTransitionNode.h"
#include "GraphEditor/Nodes/FSMGraphEntryNode.h"
#include "MultiBoxBuilder.h"
#include "GenericCommands.h"
#include "GraphEditorActions.h"
#include "ScopedTransaction.h"
#include "GraphEditor/FSMGraph.h"

#define LOCTEXT_NAMESPACE "FSMAssetEditorNativeNames"

const FText UFSMGraphSchema::NODE_CATEGORY(LOCTEXT("NodeActionCategory", "Node"));
const FText UFSMGraphSchema::CREATE_NODE_DESC(LOCTEXT("CreateNodeActionDesc", "New Node"));
const FText UFSMGraphSchema::CREATE_NODE_TOOL_TIP(LOCTEXT("CreateNodeActionToolTip", "Create a new node"));
const FText UFSMGraphSchema::GRAPH_CATEGORY(LOCTEXT("GraphActionCategory", "Graph"));

const FName UFSMGraphSchema::PC_Trans(TEXT("Transition"));
const FName UFSMGraphSchema::PC_Entry(TEXT("Entry"));

void UFSMGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder & ContextMenuBuilder) const
{
	TSharedPtr<FFSMSchemaAction_NewNode> Action(new FFSMSchemaAction_NewNode
	(
		NODE_CATEGORY,
		CREATE_NODE_DESC, 
		CREATE_NODE_TOOL_TIP,
		0
	));
	ContextMenuBuilder.AddAction(Action);
	if (ContextMenuBuilder.FromPin != nullptr) {
		//TODO: Create connection between frompin node and new created node
	}
	else
	{
		auto CurrentGraph = ContextMenuBuilder.CurrentGraph;
		// The rest of the comment actions are in the UEdGraphSchema::GetContextMenuActions
		const bool bIsManyNodesSelected = CurrentGraph ? GetNodeSelectionCount(CurrentGraph) > 0 : false;
		const FText MenuDescription = bIsManyNodesSelected ?
			LOCTEXT("CreateCommentAction", "Create Comment from Selection") : LOCTEXT("AddCommentAction", "Add Comment...");
		const FText ToolTip = LOCTEXT("CreateCommentToolTip", "Creates a comment.");
		constexpr int32 Grouping = 1;

		TSharedPtr<FFSMSchemaAction_NewComment> NewAction(new FFSMSchemaAction_NewComment(
			GRAPH_CATEGORY, MenuDescription, ToolTip, Grouping));
		ContextMenuBuilder.AddAction(NewAction);
	}
}

void UFSMGraphSchema::GetContextMenuActions(const UEdGraph * CurrentGraph, const UEdGraphNode * InGraphNode, const UEdGraphPin * InGraphPin, FMenuBuilder * MenuBuilder, bool bIsDebugging) const
{
	check(CurrentGraph);

	if (InGraphNode != NULL)
	{
		MenuBuilder->BeginSection("FSMNodeActions", LOCTEXT("NodeActionsMenuHeader", "Node Actions"));
		{
			if (!bIsDebugging)
			{
				// Node contextual actions
				MenuBuilder->AddMenuEntry(FGenericCommands::Get().Delete);
				//These three are not allowed
				//MenuBuilder->AddMenuEntry(FGenericCommands::Get().Cut);
				//MenuBuilder->AddMenuEntry(FGenericCommands::Get().Copy);
				//MenuBuilder->AddMenuEntry(FGenericCommands::Get().Duplicate);
				MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().ReconstructNodes);
				MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().BreakNodeLinks);
				if (InGraphNode->bCanRenameNode)
				{
					MenuBuilder->AddMenuEntry(FGenericCommands::Get().Rename);
				}
			}
		}
		MenuBuilder->EndSection();
	}

	Super::GetContextMenuActions(CurrentGraph, InGraphNode, InGraphPin, MenuBuilder, bIsDebugging);
}

void UFSMGraphSchema::CreateDefaultNodesForGraph(UEdGraph & Graph) const
{
	// Create the entry/exit tunnels
	FGraphNodeCreator<UFSMGraphEntryNode> NodeCreator(Graph);
	UFSMGraphEntryNode* EntryNode = NodeCreator.CreateNode();
	NodeCreator.Finalize();
	SetNodeMetaData(EntryNode, FNodeMetadata::DefaultGraphNode);

	if (UFSMGraph* FSMGraph = CastChecked<UFSMGraph>(&Graph)){
		FSMGraph->Entry = EntryNode;
	}
}

const FPinConnectionResponse UFSMGraphSchema::CanCreateConnection(const UEdGraphPin * PinA, const UEdGraphPin * PinB) const
{
	// Make sure the pins are not on the same node
	if (PinA->GetOwningNode() == PinB->GetOwningNode())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Both are on the same node"));
	}

	if (PinA->PinType.PinCategory == UFSMGraphSchema::PC_Trans && PinB->PinType.PinCategory == UFSMGraphSchema::PC_Trans) {
		return FPinConnectionResponse(ECanCreateConnectionResponse::CONNECT_RESPONSE_MAKE_WITH_CONVERSION_NODE, TEXT(""));
	}
	if (PinA->PinType.PinCategory == UFSMGraphSchema::PC_Entry && PinB->PinType.PinCategory == UFSMGraphSchema::PC_Trans) {
		return FPinConnectionResponse(ECanCreateConnectionResponse::CONNECT_RESPONSE_BREAK_OTHERS_A, TEXT(""));
	}
	if (PinA->PinType.PinCategory == UFSMGraphSchema::PC_Trans && PinB->PinType.PinCategory == UFSMGraphSchema::PC_Entry) {
		return FPinConnectionResponse(ECanCreateConnectionResponse::CONNECT_RESPONSE_DISALLOW, TEXT("Don't allow Transition into 'Entry', because 'Entry' is not a state"));
	}
	return FPinConnectionResponse(ECanCreateConnectionResponse::CONNECT_RESPONSE_DISALLOW, TEXT("Unknown pin type."));
}

bool UFSMGraphSchema::TryCreateConnection(UEdGraphPin * PinA, UEdGraphPin * PinB) const
{
	if (PinB->Direction == PinA->Direction)
	{
		if (UFSMGraphNode* Node = Cast<UFSMGraphNode>(PinB->GetOwningNode()))
		{
			if (PinA->Direction == EGPD_Input)
			{
				PinB = Node->GetOutputPin();
			}
			else
			{
				PinB = Node->GetInputPin();
			}
		}
	}

	const bool bModified = UEdGraphSchema::TryCreateConnection(PinA, PinB);
	/*
	if (bModified)
	{
		UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForNodeChecked(PinA->GetOwningNode());
		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
	}
	*/
	return bModified;
}

bool UFSMGraphSchema::CreateAutomaticConversionNodeAndConnections(UEdGraphPin * PinA, UEdGraphPin * PinB) const
{
	//Reference: UAnimationStateMachineSchema::CreateAutomaticConversionNodeAndConnections
	UFSMGraphNode* NodeA = Cast<UFSMGraphNode>(PinA->GetOwningNode());
	UFSMGraphNode* NodeB = Cast<UFSMGraphNode>(PinB->GetOwningNode());

	FName TransName("Name_Temp");

	if ((NodeA != NULL) && (NodeB != NULL)
		&& (NodeA->GetInputPin() != NULL) && (NodeA->GetOutputPin() != NULL)
		&& (NodeB->GetInputPin() != NULL) && (NodeB->GetOutputPin() != NULL))
	{
		UFSMTransitionNode* TransitionNode = FEdGraphSchemaAction_NewNode::SpawnNodeFromTemplate<UFSMTransitionNode>(NodeA->GetGraph(), NewObject<UFSMTransitionNode>(), FVector2D(0.0f, 0.0f), false);

		if (PinA->Direction == EGPD_Output)
		{
			return TransitionNode->CreateConnections(NodeA, NodeB, TransName);
		}
		else
		{
			return TransitionNode->CreateConnections(NodeB, NodeA, TransName);
		}
		/*
		UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraphChecked(TransitionNode->GetBoundGraph());
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
		*/
	}
	return false;
}

void UFSMGraphSchema::BreakNodeLinks(UEdGraphNode& TargetNode) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("FSMAssetEditorNativeNames", "GraphEd_BreakNodeLinks", "Break Node Links"));

	Super::BreakNodeLinks(TargetNode);
}

void UFSMGraphSchema::BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotification) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("FSMAssetEditorNativeNames", "GraphEd_BreakPinLinks", "Break Pin Links"));

	Super::BreakPinLinks(TargetPin, bSendsNodeNotification);
}

void UFSMGraphSchema::BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("FSMAssetEditorNativeNames", "GraphEd_BreakSinglePinLink", "Break Pin Link"));

	Super::BreakSinglePinLink(SourcePin, TargetPin);
}
#undef LOCTEXT_NAMESPACE
