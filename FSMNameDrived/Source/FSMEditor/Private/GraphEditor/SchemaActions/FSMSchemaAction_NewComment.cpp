#include "GraphEditor/SchemaActions/FSMSchemaAction_NewComment.h"
#include "SlateRect.h"
#include "EdGraphNode_Comment.h"
#include "FSMAssetEditor.h"
#include "GraphEditor/FSMGraph.h"
#include "ToolkitManager.h"
#include "FSM.h"

#define LOCTEXT_NAMESPACE "FSMAssetEditorNativeNames"
UEdGraphNode * FFSMSchemaAction_NewComment::PerformAction(UEdGraph * ParentGraph, UEdGraphPin * FromPin, const FVector2D Location, bool bSelectNewNode)
{
	// Add menu item for creating comment boxes
	UEdGraphNode_Comment* CommentTemplate = NewObject<UEdGraphNode_Comment>();

	TSharedPtr<FFSMAssetEditor> Editor;
	auto FSM = Cast<UFSMGraph>(ParentGraph)->GetFSM();
	TSharedPtr<IToolkit> FoundAssetEditor = FToolkitManager::Get().FindEditorForAsset(FSM);
	if (FoundAssetEditor.IsValid())
	{
		Editor = StaticCastSharedPtr<FFSMAssetEditor>(FoundAssetEditor);
	}

	// Wrap comment around other nodes, this makes it possible to select other nodes and press the "C" key on the keyboard.
	FVector2D SpawnLocation = Location;
	FSlateRect Bounds;
	if (Editor->GetBound(Bounds, 50.0f))
	{
		CommentTemplate->SetBounds(Bounds);
		SpawnLocation.X = CommentTemplate->NodePosX;
		SpawnLocation.Y = CommentTemplate->NodePosY;
	}

	return FEdGraphSchemaAction_NewNode::SpawnNodeFromTemplate<UEdGraphNode_Comment>(ParentGraph, CommentTemplate, SpawnLocation, bSelectNewNode);
}
#undef LOCTEXT_NAMESPACE