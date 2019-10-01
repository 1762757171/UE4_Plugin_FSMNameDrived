// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NotifyHook.h"
#include "EditorUndoClient.h"
#include "AssetEditorToolkit.h"
#include "GraphEditor.h"

/**
 * 
 */
class FSMEDITOR_API FFSMAssetEditor :public FNotifyHook, public FEditorUndoClient, public FAssetEditorToolkit
{
public:
	FFSMAssetEditor();
	~FFSMAssetEditor();

	void InitializeAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, class UFSM* ObjectToEdit);

	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;

	virtual FName GetToolkitFName() const override { return FName("FSMAssetEditor"); }
#define LOCTEXT_NAMESPACE "FSMEditorNativeNames" 
	virtual FText GetBaseToolkitName() const override { return LOCTEXT("BaseToolKitName", "FSMAssetEditor"); }
#undef LOCTEXT_NAMESPACE 
	virtual FString GetWorldCentricTabPrefix() const override { return "GameplayCustomized"; }
	virtual FLinearColor GetWorldCentricTabColorScale() const override { return FLinearColor::White; }

	/** Called when a node's title is committed for a rename */
	void OnNodeTitleCommitted(const FText& NewText, ETextCommit::Type CommitInfo, UEdGraphNode* NodeBeingChanged);
	bool OnNodeVerifyTitleCommit(const FText& NewText, UEdGraphNode* NodeBeingChanged, FText& OutErrorMessage);

public:
	bool GetBound(class FSlateRect& Rect, float Padding);

private:
	void OnRenameNode();
	bool CanRenameNodes() const;

	void SelectAllNodes();
	bool CanSelectAllNodes() const;

	void DeleteSelectedNodes();
	bool CanDeleteNodes() const;

	/* Create comment node on graph */
	void OnCreateComment();
private:
	TSharedRef<SDockTab> SpawnTab_GraphCanvas(const FSpawnTabArgs& Args) const;

	TSharedRef<class SGraphEditor> CreateGraphEditorWidget();

	//void OnNodeNameCommitted(const FText& NewText, ETextCommit::Type CommitInfo, UEdGraphNode* NodeBeingChanged) const;
	FActionMenuContent OnCreateGraphActionMenu(UEdGraph* Graph, const FVector2D& NodePosition, const TArray<UEdGraphPin*>& DraggedPins, bool bAutoExpand, SGraphEditor::FActionMenuClosed OnMenuClosed);
	void OnGraphActionMenuClosed(bool bActionExecuted, bool bGraphPinContext);

	FGraphPanelSelectionSet GetSelectedNodes() const;
	UEdGraphNode* GetFirstSelectedNode() const;
private:
	TSharedPtr<class SGraphEditor> GraphEditorView;

	class UFSM* TargetFSM;
	
	/** Command list for the graph editor */
	TSharedPtr<class FUICommandList> GraphEditorCommands;
private:
	static const FName TabID_EditorGraphCanvas;
};
