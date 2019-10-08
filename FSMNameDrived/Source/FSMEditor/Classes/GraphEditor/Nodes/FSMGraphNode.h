#pragma once

#include "CoreTypes.h"
#include "UObject/ObjectMacros.h"
#include "EdGraph/EdGraphNode.h"
#include "SGraphNode.h"

#include "FSMGraphNode.generated.h"

UCLASS()
class UFSMGraphNode : public UEdGraphNode
{
	GENERATED_BODY()

public:
	UFSMGraphNode();

	virtual bool CanUserDeleteNode() const override { return true; }

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void AllocateDefaultPins() override;
	virtual void DestroyNode() override;
	//virtual void PostPasteNode() override;
	//virtual FString GetDocumentationLink() const override;
	virtual TSharedPtr<class INameValidatorInterface> MakeNameValidator() const override;
	virtual void OnRenameNode(const FString& NewName) override;

	FSMEDITOR_API void SetName(FName NewName);

	FSMEDITOR_API UEdGraphPin* GetOutputPin() const;
	FSMEDITOR_API UEdGraphPin* GetInputPin() const;

	FSMEDITOR_API FName GetNodeName() const;
private:
	FSMEDITOR_API class UFSM* GetFSM() const;
private:
	UPROPERTY()
	FName NodeName;

	static const int32 INPUT_PIN_INDEX;
	static const int32 OUTPUT_PIN_INDEX;
};

class SFSMGraphNode : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SFSMGraphNode) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UFSMGraphNode* InNode);

	// SGraphNode interface
	virtual void UpdateGraphNode() override;
	virtual void CreatePinWidgets() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	virtual TSharedPtr<SToolTip> GetComplexTooltip() override;
	// End of SGraphNode interface

};