#pragma once

#include "Widgets/DeclarativeSyntaxSupport.h"
#include "SGraphPalette.h"
#include "GraphEditor.h"
#include "SGraphActionMenu.h"

class SFSMActionMenu : public SBorder
{
public:
	DECLARE_DELEGATE_TwoParams(FFSMActionMenuCloseReason, bool /*bActionExecuted*/, bool /*bGraphPinContext*/);

	SLATE_BEGIN_ARGS(SFSMActionMenu)
		: _Graph(nullptr)
		, _NewNodePosition(FVector2D::ZeroVector)
		, _AutoExpandActionMenu(true)
	{}

	SLATE_ARGUMENT(UEdGraph*, Graph);
	SLATE_ARGUMENT(FVector2D, NewNodePosition);
	SLATE_ARGUMENT(TArray<UEdGraphPin*>, DraggedFromPins);
	SLATE_ARGUMENT(SGraphEditor::FActionMenuClosed, OnClosedCallback);
	SLATE_ARGUMENT(bool, AutoExpandActionMenu);
	SLATE_EVENT(FFSMActionMenuCloseReason, OnCloseReason);
	SLATE_END_ARGS();

	void Construct(const FArguments& InArgs);

	~SFSMActionMenu();

	TSharedPtr<SEditableTextBox> GetFilterTextBox() const { return GraphActionMenu->GetFilterTextBox(); }

protected:
	void OnActionSelected(const TArray<TSharedPtr<FEdGraphSchemaAction>>& SelectedAction, ESelectInfo::Type InSelectionType);

	/** Callback used to populate all actions list in SGraphActionMenu */
	void CollectAllActions(FGraphActionListBuilderBase& OutAllActions);

protected:
	UEdGraph* Graph = nullptr;

	/** The pins this node was dragged from. */
	TArray<UEdGraphPin*> DraggedFromPins;

	/** The position of this new node */
	FVector2D NewNodePosition;

	/** Should expand the new menu? */
	bool AutoExpandActionMenu = true;

	/** Was an action executed? */
	bool bActionExecuted = false;

	// Events for close
	SGraphEditor::FActionMenuClosed OnClosedCallback;
	FFSMActionMenuCloseReason OnCloseReasonCallback;

	// The actual menu
	TSharedPtr<SGraphActionMenu> GraphActionMenu;
};