#pragma once

#include "EdGraph/EdGraphSchema.h"
#include "Templates/SubclassOf.h"

#include "FSMSchemaAction_NewComment.generated.h"

/** Action to add a comment to the graph */
USTRUCT()
struct FSMEDITOR_API FFSMSchemaAction_NewComment : public FEdGraphSchemaAction
{
	GENERATED_BODY()
public:
	FFSMSchemaAction_NewComment() : FEdGraphSchemaAction() {}
	FFSMSchemaAction_NewComment(const FText& InNodeCategory, const FText& InMenuDesc, const FText& InToolTip, int32 InGrouping)
		: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping) {}

public:
	UEdGraphNode* PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
};