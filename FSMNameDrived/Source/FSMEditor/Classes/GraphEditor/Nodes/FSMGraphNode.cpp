#include "FSMGraphNode.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Images/SImage.h"
#include "SGraphPreviewer.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "IDocumentation.h"
#include "SInlineEditableTextBlock.h"
#include "GraphEditor/Pins/FSMGraphPin.h"
#include "FSM.h"
#include "GraphEditor/FSMGraph.h"
#include "ScopedTransaction.h"
#include "GraphEditor/NameValidators/FSMNodeNameValidator.h"

#define LOCTEXT_NAMESPACE "FSMAssetEditorNativeNames"

//Reference: D:\Unreal Engine\UE_4.21\Engine\Source\Editor\AnimationBlueprintEditor\Private\AnimationStateNodes\SGraphNodeAnimState.cpp

const int32 UFSMGraphNode::INPUT_PIN_INDEX = 0;
const int32 UFSMGraphNode::OUTPUT_PIN_INDEX = 1;


void SFSMGraphNode::Construct(const FArguments & InArgs, UFSMGraphNode * InNode)
{
	SetCursor(EMouseCursor::CardinalCross);

	GraphNode = InNode;
	
	UpdateGraphNode();
}

void SFSMGraphNode::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();

	// Reset variables that are going to be exposed, in case we are refreshing an already setup node.
	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	FLinearColor TitleShadowColor(0.6f, 0.6f, 0.6f);
	TSharedPtr<SErrorText> ErrorText;
	TSharedPtr<SNodeTitle> NodeTitle = SNew(SNodeTitle, GraphNode);

	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
	this->GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("Graph.StateNode.Body"))
			.Padding(0)
			//.BorderBackgroundColor(this, &SGraphNodeAnimState::GetBorderBackgroundColor)
			.BorderBackgroundColor(FLinearColor::Gray)
			[
				SNew(SOverlay)

				// PIN AREA
				+ SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SAssignNew(RightNodeBox, SVerticalBox)
				]

				// STATE NAME AREA
				+ SOverlay::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.Padding(10.0f)
				[
					SNew(SBorder)
					.BorderImage(FEditorStyle::GetBrush("Graph.StateNode.ColorSpill"))
					.BorderBackgroundColor(TitleShadowColor)
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					.Visibility(EVisibility::SelfHitTestInvisible)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							// POPUP ERROR MESSAGE
							SAssignNew(ErrorText, SErrorText)
							.BackgroundColor(this, &SFSMGraphNode::GetErrorColor)
							.ToolTipText(this, &SFSMGraphNode::GetErrorMsgToolTip)
						]
						/*+ SHorizontalBox::Slot()
						.AutoWidth()
						.VAlign(VAlign_Center)
						[
							SNew(SImage)
							.Image(FEditorStyle::GetBrush(TEXT("Graph.StateNode.Icon")))
						]*/
						+ SHorizontalBox::Slot()
						.Padding(FMargin(4.0f, 0.0f, 4.0f, 0.0f))
						[
							SNew(SVerticalBox)
							+ SVerticalBox::Slot()
							.AutoHeight()
							[
								SAssignNew(InlineEditableText, SInlineEditableTextBlock)
								.Style(FEditorStyle::Get(), "Graph.StateNode.NodeTitleInlineEditableText")
								.Text(NodeTitle.Get(), &SNodeTitle::GetHeadTitle)
								.OnVerifyTextChanged(this, &SFSMGraphNode::OnVerifyNameTextChanged)
								.OnTextCommitted(this, &SFSMGraphNode::OnNameTextCommited)
								.IsReadOnly(this, &SFSMGraphNode::IsNameReadOnly)
								.IsSelected(this, &SFSMGraphNode::IsSelectedExclusively)
							]
							+ SVerticalBox::Slot()
							.AutoHeight()
							[
								NodeTitle.ToSharedRef()
							]
						]
					]
				]
			]
		];

	ErrorReporting = ErrorText;
	ErrorReporting->SetError(ErrorMsg);
	CreatePinWidgets();
}

void SFSMGraphNode::CreatePinWidgets()
{
	UFSMGraphNode* StateNode = CastChecked<UFSMGraphNode>(GraphNode);

	UEdGraphPin* CurPin = StateNode->GetOutputPin();
	if (!CurPin->bHidden)
	{
		TSharedPtr<SGraphPin> NewPin = SNew(SFSMGraphPin, CurPin);

		this->AddPin(NewPin.ToSharedRef());
	}
}

void SFSMGraphNode::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	PinToAdd->SetOwner(SharedThis(this));
	RightNodeBox->AddSlot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.FillHeight(1.0f)
		[
			PinToAdd
		];
	OutputPins.Add(PinToAdd);
}

TSharedPtr<SToolTip> SFSMGraphNode::GetComplexTooltip()
{
	return nullptr;
}

FText UFSMGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromName(NodeName);
}

void UFSMGraphNode::AllocateDefaultPins()
{
	UEdGraphPin* InputPin = CreatePin(EGPD_Input, TEXT("Transition"), TEXT("In"));
	UEdGraphPin* OutputPin = CreatePin(EGPD_Output, TEXT("Transition"), TEXT("Out"));
}

void UFSMGraphNode::DestroyNode()
{
	if (!GetFSM()) {
		return;
	}
	const FScopedTransaction Transaction(LOCTEXT("EditorDestroyNode", "FSM Editor: Destroy Node"));
	GetFSM()->Modify();
	if (GetFSM()->DeleteState(NodeName)) {
		Super::DestroyNode();
	}
	else {
		//If Goes here
		//Graph goes wrong!!!
		//Must solve this problem!!!!
	}
}

TSharedPtr<class INameValidatorInterface> UFSMGraphNode::MakeNameValidator() const
{
	return MakeShareable(new FFSMNodeNameValidator(this));
}

void UFSMGraphNode::OnRenameNode(const FString & NewName)
{
	if (!GetFSM()) {
		return;
	}
	const FScopedTransaction Transaction(LOCTEXT("EditorRenameNode", "FSM Editor: Rename Node"));
	//NameValidator->FindValidString(NodeName);
	GetFSM()->Modify();
	if (GetFSM()->RenameState(NodeName, FName(*NewName))) {
		NodeName = FName(*NewName);
	}
}

void UFSMGraphNode::SetName(FName NewName)
{
	NodeName = NewName;
}

UEdGraphPin * UFSMGraphNode::GetOutputPin() const
{
	return Pins[OUTPUT_PIN_INDEX];
}

UEdGraphPin * UFSMGraphNode::GetInputPin() const
{
	return Pins[INPUT_PIN_INDEX];
}

FName UFSMGraphNode::GetNodeName() const
{
	return NodeName;
}

UFSM * UFSMGraphNode::GetFSM() const
{
	UFSMGraph* Graph = Cast<UFSMGraph>(GetGraph());
	return Graph->GetFSM();
}

UFSMGraphNode::UFSMGraphNode()
{
	bCanRenameNode = true;
}

#undef LOCTEXT_NAMESPACE