#pragma once

#include "EdGraph/EdGraphSchema.h"
#include "Templates/SubclassOf.h"

#include "FSMSchemaAction_NewNode.generated.h"

/** Action to add a node to the graph */
USTRUCT()
struct FSMEDITOR_API FFSMSchemaAction_NewNode : public FEdGraphSchemaAction
{
	GENERATED_BODY()
public:
	FFSMSchemaAction_NewNode() : FEdGraphSchemaAction() {}
	FFSMSchemaAction_NewNode(const FText& InNodeCategory, const FText& InMenuDesc, const FText& InToolTip,int32 InGrouping)
		: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping) {}

public:
	UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
};