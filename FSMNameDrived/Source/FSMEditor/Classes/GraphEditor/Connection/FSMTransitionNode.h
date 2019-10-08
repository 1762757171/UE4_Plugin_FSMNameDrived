#pragma once
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "EdGraph/EdGraphNode.h"
#include "SGraphNode.h"

#include "FSMTransitionNode.generated.h"

UCLASS(MinimalAPI, config = Editor)
class UFSMTransitionNode : public UEdGraphNode
{
	GENERATED_BODY()

public:
	UFSMTransitionNode();

	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override; 
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;
	virtual void DestroyNode() override;
	//virtual void PostPasteNode() override;
	//virtual FString GetDocumentationLink() const override;
	virtual TSharedPtr<class INameValidatorInterface> MakeNameValidator() const override;
	virtual void OnRenameNode(const FString& NewName) override;

	FSMEDITOR_API UEdGraphPin* GetInputPin() const;
	FSMEDITOR_API UEdGraphPin* GetOutputPin() const;

	FSMEDITOR_API class UFSMGraphNode* GetFromNode() const;
	FSMEDITOR_API class UFSMGraphNode* GetTargetNode() const;

	FSMEDITOR_API void SetTransitionName(FName NewName);
	FSMEDITOR_API FName GetTransitionName() const;

	FSMEDITOR_API bool CreateConnections(class UFSMGraphNode* PreviousState, class UFSMGraphNode* NextState, FName TransName);
private:
	class UFSM* GetFSM() const;
private:
	UPROPERTY()
	FName Transition;
	UPROPERTY()
	class UFSMGraphNode* FromNode;
	UPROPERTY()
	class UFSMGraphNode* TargetNode;
private:
	static const int32 INPUT_PIN_INDEX;
	static const int32 OUTPUT_PIN_INDEX;
};

class SFSMTransitionNode : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SFSMTransitionNode) {}
	SLATE_END_ARGS()
public:
	void Construct(const FArguments& InArgs, UFSMTransitionNode* InNode);
public:
	virtual void UpdateGraphNode() override;
	virtual void MoveTo(const FVector2D& NewPosition, FNodeSet& NodeFilter) override;
	virtual bool RequiresSecondPassLayout() const override;
	virtual void PerformSecondPassLayout(const TMap< UObject*, TSharedRef<SNode> >& NodeToWidgetLookup) const override;

	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;
private:
	void PositionBetweenTwoNodesWithOffset(const FGeometry& StartGeom, const FGeometry& EndGeom, int32 NodeIndex, int32 MaxNodes) const;
	FSlateColor GetTransitionColor() const;
};