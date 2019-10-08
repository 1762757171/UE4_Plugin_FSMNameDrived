#include "GraphEditor/Connection/FSMTransitionNode.h"
#include "EdGraph/EdGraph.h"
#include "GraphEditor/Nodes/FSMGraphNode.h"
#include "ScopedTransaction.h"
#include "FSM.h"
#include "GraphEditor/FSMGraph.h"
#include "ConnectionDrawingPolicy.h"
#include "SGraphPanel.h"
#include "SImage.h"
#include "SInlineEditableTextBlock.h"
#include "Widgets/SBoxPanel.h"
#include "SErrorText.h"
#include "GraphEditor/NameValidators/FSMTransitionNameValidator.h"

#define LOCTEXT_NAMESPACE "FSMAssetEditorNativeNames"

const int32 UFSMTransitionNode::INPUT_PIN_INDEX = 0;
const int32 UFSMTransitionNode::OUTPUT_PIN_INDEX = 1;

void UFSMTransitionNode::AllocateDefaultPins()
{
	UEdGraphPin* Inputs = CreatePin(EGPD_Input, TEXT("Transition"), TEXT("In"));
	Inputs->bHidden = true;
	UEdGraphPin* Outputs = CreatePin(EGPD_Output, TEXT("Transition"), TEXT("Out"));
	Outputs->bHidden = true;
}

FText UFSMTransitionNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromName(Transition);
}

FLinearColor UFSMTransitionNode::GetNodeTitleColor() const
{
	return FLinearColor::Gray;
}

void UFSMTransitionNode::PinConnectionListChanged(UEdGraphPin * Pin)
{
	if (Pin->LinkedTo.Num() == 0)
	{
		// Commit suicide; transitions must always have an input and output connection
		Modify();

		// Our parent graph will have our graph in SubGraphs so needs to be modified to record that.
		if (UEdGraph* ParentGraph = GetGraph())
		{
			ParentGraph->Modify();
		}

		DestroyNode();
	}
}

void UFSMTransitionNode::DestroyNode()
{
	if (!GetFSM()) {
		return;
	}
	const FScopedTransaction Transaction(LOCTEXT("EditorDestroyTransition", "FSM Editor: Destroy Transition"));
	GetFSM()->Modify();
	if (GetFSM()->DeleteRule(FromNode->GetNodeName(), Transition)) {
		Super::DestroyNode();
	}
	else {
		//Graph goes wrong!!!
		//Must solve this problem!!!!
	}
}

TSharedPtr<class INameValidatorInterface> UFSMTransitionNode::MakeNameValidator() const
{
	return MakeShareable(new FFSMTransitionNameValidator(this));
}

void UFSMTransitionNode::OnRenameNode(const FString & NewName)
{
	if (!GetFSM()) {
		return;
	}
	const FScopedTransaction Transaction(LOCTEXT("EditorDestroyTransition", "FSM Editor: Destroy Transition"));
	GetFSM()->Modify();
	if (GetFSM()->RenameRule(FromNode->GetNodeName(), Transition, FName(*NewName))) {
		Transition = FName(*NewName);
	}
}

UEdGraphPin * UFSMTransitionNode::GetInputPin() const
{
	return Pins[INPUT_PIN_INDEX];
}

UEdGraphPin * UFSMTransitionNode::GetOutputPin() const
{
	return Pins[OUTPUT_PIN_INDEX];
}

UFSMGraphNode * UFSMTransitionNode::GetFromNode() const
{
	return FromNode;
}

UFSMGraphNode * UFSMTransitionNode::GetTargetNode() const
{
	return TargetNode;
}

void UFSMTransitionNode::SetTransitionName(FName NewName)
{
	Transition = NewName;
}

FName UFSMTransitionNode::GetTransitionName() const
{
	return Transition;
}

FSMEDITOR_API bool UFSMTransitionNode::CreateConnections(UFSMGraphNode* PreviousState, UFSMGraphNode* NextState, FName TransName)
{
	FromNode = PreviousState;
	TargetNode = NextState;

	SetTransitionName(TransName);

	const FScopedTransaction Transaction(LOCTEXT("EditorNewTransition", "FSM Editor: New Transition"));
	UFSM* FSM = CastChecked<UFSMGraph>(GetGraph())->GetFSM();
	if (!FSM->AddRule(Transition, FromNode->GetNodeName(), TargetNode->GetNodeName())) {
		return false;
	}

	// Previous to this
	GetInputPin()->Modify();
	GetInputPin()->LinkedTo.Empty();

	PreviousState->GetOutputPin()->Modify();
	//GetInputPin()->MakeLinkTo(PreviousState->GetOutputPin());
	PreviousState->GetOutputPin()->MakeLinkTo(GetInputPin());

	// This to next
	GetOutputPin()->Modify();
	GetOutputPin()->LinkedTo.Empty();

	NextState->GetInputPin()->Modify();
	GetOutputPin()->MakeLinkTo(NextState->GetInputPin());

	return true;
}

UFSM * UFSMTransitionNode::GetFSM() const
{
	UFSMGraph* Graph = Cast<UFSMGraph>(GetGraph());
	return Graph->GetFSM();
}

void SFSMTransitionNode::Construct(const FArguments& InArgs, UFSMTransitionNode* InNode)
{
	GraphNode = InNode;
	UpdateGraphNode();
}

void SFSMTransitionNode::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();

	// Reset variables that are going to be exposed, in case we are refreshing an already setup node.
	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	FLinearColor TitleShadowColor(0.4f, 0.4f, 0.4f);
	TSharedPtr<SErrorText> ErrorText;
	TSharedPtr<SNodeTitle> NodeTitle = SNew(SNodeTitle, GraphNode);

	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
	this->GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SOverlay)
			// STATE NAME AREA
			+ SOverlay::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.Padding(0.0f)
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
						.BackgroundColor(this, &SFSMTransitionNode::GetErrorColor)
						.ToolTipText(this, &SFSMTransitionNode::GetErrorMsgToolTip)
					]
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
							.OnVerifyTextChanged(this, &SFSMTransitionNode::OnVerifyNameTextChanged)
							.OnTextCommitted(this, &SFSMTransitionNode::OnNameTextCommited)
							.IsReadOnly(this, &SFSMTransitionNode::IsNameReadOnly)
							.IsSelected(this, &SFSMTransitionNode::IsSelectedExclusively)
						]
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							NodeTitle.ToSharedRef()
						]
					]
				]
			]
		];
}

void SFSMTransitionNode::MoveTo(const FVector2D & NewPosition, FNodeSet & NodeFilter)
{
	//Override to ignore, position decided by connection line.
}

bool SFSMTransitionNode::RequiresSecondPassLayout() const
{
	return true;
}

void SFSMTransitionNode::PerformSecondPassLayout(const TMap< UObject*, TSharedRef<SNode> >& NodeToWidgetLookup) const
{
	UFSMTransitionNode* TransNode = CastChecked<UFSMTransitionNode>(GraphNode);

	// Find the geometry of the state nodes we're connecting
	FGeometry StartGeom;
	FGeometry EndGeom;

	int32 TransIndex = 0;
	int32 NumOfTrans = 1;

	UFSMGraphNode* PrevState = TransNode->GetFromNode();
	UFSMGraphNode* NextState = TransNode->GetTargetNode();
	if ((PrevState != NULL) && (NextState != NULL))
	{
		const TSharedRef<SNode>* pPrevNodeWidget = NodeToWidgetLookup.Find(PrevState);
		const TSharedRef<SNode>* pNextNodeWidget = NodeToWidgetLookup.Find(NextState);
		if ((pPrevNodeWidget != NULL) && (pNextNodeWidget != NULL))
		{
			const TSharedRef<SNode>& PrevNodeWidget = *pPrevNodeWidget;
			const TSharedRef<SNode>& NextNodeWidget = *pNextNodeWidget;

			StartGeom = FGeometry(FVector2D(PrevState->NodePosX, PrevState->NodePosY), FVector2D::ZeroVector, PrevNodeWidget->GetDesiredSize(), 1.0f);
			EndGeom = FGeometry(FVector2D(NextState->NodePosX, NextState->NodePosY), FVector2D::ZeroVector, NextNodeWidget->GetDesiredSize(), 1.0f);

			TArray<UFSMTransitionNode*> Transitions;

			//Get all transitions linked PrevState->GetOutputPin()
			for (int32 LinkIndex = 0; LinkIndex < PrevState->GetOutputPin()->LinkedTo.Num(); ++LinkIndex)
			{
				UEdGraphNode* TargetNode = PrevState->GetOutputPin()->LinkedTo[LinkIndex]->GetOwningNode();
				if (UFSMTransitionNode* Transition = Cast<UFSMTransitionNode>(TargetNode))
				{
					Transitions.Add(Transition);
				}
			}

			Transitions = Transitions.FilterByPredicate([NextState](const UFSMTransitionNode* InTransition) -> bool
			{
				return InTransition->GetTargetNode() == NextState;
			});

			TransIndex = Transitions.IndexOfByKey(TransNode);
			NumOfTrans = Transitions.Num();
		}
	}

	//Position Node
	PositionBetweenTwoNodesWithOffset(StartGeom, EndGeom, TransIndex, NumOfTrans);
}

void SFSMTransitionNode::PositionBetweenTwoNodesWithOffset(const FGeometry& StartGeom, const FGeometry& EndGeom, int32 NodeIndex, int32 MaxNodes) const
{
	// Get a reasonable seed point (halfway between the boxes)
	const FVector2D StartCenter = FGeometryHelper::CenterOf(StartGeom);
	const FVector2D EndCenter = FGeometryHelper::CenterOf(EndGeom);
	const FVector2D SeedPoint = (StartCenter + EndCenter) * 0.5f;

	// Find the (approximate) closest points between the two boxes
	const FVector2D StartAnchorPoint = FGeometryHelper::FindClosestPointOnGeom(StartGeom, SeedPoint);
	const FVector2D EndAnchorPoint = FGeometryHelper::FindClosestPointOnGeom(EndGeom, SeedPoint);

	// Position ourselves halfway along the connecting line between the nodes, elevated away perpendicular to the direction of the line
	const float Height = 30.0f;

	const FVector2D DesiredNodeSize = GetDesiredSize();

	FVector2D DeltaPos(EndAnchorPoint - StartAnchorPoint);

	if (DeltaPos.IsNearlyZero())
	{
		DeltaPos = FVector2D(10.0f, 0.0f);
	}

	const FVector2D Normal = FVector2D(DeltaPos.Y, -DeltaPos.X).GetSafeNormal();

	const FVector2D NewCenter = StartAnchorPoint + (0.5f * DeltaPos) + (Height * Normal);

	FVector2D DeltaNormal = DeltaPos.GetSafeNormal();

	// Calculate node offset in the case of multiple transitions between the same two nodes
	// MultiNodeOffset: the offset where 0 is the centre of the transition, -1 is 1 <size of node>
	// towards the PrevStateNode and +1 is 1 <size of node> towards the NextStateNode.

	const float MutliNodeSpace = 0.2f; // Space between multiple transition nodes (in units of <size of node> )
	const float MultiNodeStep = (1.f + MutliNodeSpace); //Step between node centres (Size of node + size of node spacer)

	const float MultiNodeStart = -((MaxNodes - 1) * MultiNodeStep) / 2.f;
	const float MultiNodeOffset = MultiNodeStart + (NodeIndex * MultiNodeStep);

	// Now we need to adjust the new center by the node size, zoom factor and multi node offset
	const FVector2D NewCorner = NewCenter - (0.5f * DesiredNodeSize) + (DeltaNormal * MultiNodeOffset * DesiredNodeSize.Size());

	GraphNode->NodePosX = NewCorner.X;
	GraphNode->NodePosY = NewCorner.Y;
}

FSlateColor SFSMTransitionNode::GetTransitionColor() const
{
	const FLinearColor HoverColor(0.724f, 0.256f, 0.0f, 1.0f);
	FLinearColor BaseColor(0.9f, 0.9f, 0.9f, 1.0f);
	return IsHovered() ? HoverColor : BaseColor;
}

void SFSMTransitionNode::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	UFSMTransitionNode* TransNode = CastChecked<UFSMTransitionNode>(GraphNode);
	if (UEdGraphPin* Pin = TransNode->GetInputPin())
	{
		GetOwnerPanel()->AddPinToHoverSet(Pin);
	}

	SGraphNode::OnMouseEnter(MyGeometry, MouseEvent);
}

void SFSMTransitionNode::OnMouseLeave(const FPointerEvent& MouseEvent)
{
	UFSMTransitionNode* TransNode = CastChecked<UFSMTransitionNode>(GraphNode);
	if (UEdGraphPin* Pin = TransNode->GetInputPin())
	{
		GetOwnerPanel()->RemovePinFromHoverSet(Pin);
	}

	SGraphNode::OnMouseLeave(MouseEvent);
}

UFSMTransitionNode::UFSMTransitionNode()
{
	bCanRenameNode = true;
}

#undef LOCTEXT_NAMESPACE