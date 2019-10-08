#include "GraphEditor/Connection/FSMConnectionDrawingPolicy.h"
#include "DrawElements.h"
#include "GraphEditor/Connection/FSMTransitionNode.h"
#include "GraphEditor/Nodes/FSMGraphNode.h"
#include "GraphEditor/Nodes/FSMGraphEntryNode.h"

//Reference: D:\Unreal Engine\UE_4.21\Engine\Source\Editor\AnimationBlueprintEditor\Private\AnimationStateNodes\SGraphNodeAnimTransition.cpp

FFSMConnectionDrawingPolicy::FFSMConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float ZoomFactor, const FSlateRect & InClippingRect, FSlateWindowElementList & InDrawElements, UEdGraph * InGraphObj)
	:FConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, ZoomFactor, InClippingRect, InDrawElements)
{}

void FFSMConnectionDrawingPolicy::DetermineWiringStyle(UEdGraphPin * OutputPin, UEdGraphPin * InputPin, FConnectionParams & Params)
{
	Params.AssociatedPin1 = OutputPin;
	Params.AssociatedPin2 = InputPin;
	Params.WireThickness = 1.5f;

	Params.WireColor = GetTransitionColor(HoveredPins.Contains(InputPin));

	const bool bDeemphasizeUnhoveredPins = HoveredPins.Num() > 0;
	if (bDeemphasizeUnhoveredPins)
	{
		ApplyHoverDeemphasis(OutputPin, InputPin, Params.WireThickness, Params.WireColor);
	}
}

void FFSMConnectionDrawingPolicy::Draw(TMap<TSharedRef<SWidget>, FArrangedWidget>& PinGeometries, FArrangedChildren & ArrangedNodes)
{	
	// Now draw
	FConnectionDrawingPolicy::Draw(PinGeometries, ArrangedNodes);
}

void FFSMConnectionDrawingPolicy::DetermineLinkGeometry(FArrangedChildren & ArrangedNodes, TSharedRef<SWidget>& OutputPinWidget, UEdGraphPin * OutputPin, UEdGraphPin * InputPin, FArrangedWidget *& StartWidgetGeometry, FArrangedWidget *& EndWidgetGeometry)
{
	if (UFSMGraphEntryNode* EntryNode = Cast<UFSMGraphEntryNode>(OutputPin->GetOwningNode()))
	{
		StartWidgetGeometry = PinGeometries->Find(OutputPinWidget);

		UFSMGraphNode* State = CastChecked<UFSMGraphNode>(InputPin->GetOwningNode());
		for (int32 NodeIndex = 0; NodeIndex < ArrangedNodes.Num(); ++NodeIndex) {
			FArrangedWidget& CurWidget = ArrangedNodes[NodeIndex];
			TSharedRef<SGraphNode> ChildNode = StaticCastSharedRef<SGraphNode>(CurWidget.Widget);
			if (ChildNode->GetNodeObj() == State) {
				EndWidgetGeometry = &(ArrangedNodes[NodeIndex]);
			}
		}
	}
	else if (UFSMTransitionNode* TransNode = Cast<UFSMTransitionNode>(InputPin->GetOwningNode()))
	{
		UFSMGraphNode* PrevState = TransNode->GetFromNode();
		UFSMGraphNode* NextState = TransNode->GetTargetNode();
		if ((PrevState != NULL) && (NextState != NULL)){
			for (int32 NodeIndex = 0; NodeIndex < ArrangedNodes.Num(); ++NodeIndex){
				FArrangedWidget& CurWidget = ArrangedNodes[NodeIndex];
				TSharedRef<SGraphNode> ChildNode = StaticCastSharedRef<SGraphNode>(CurWidget.Widget);
				if (ChildNode->GetNodeObj() == PrevState) {
					StartWidgetGeometry = &(ArrangedNodes[NodeIndex]);
				}
				if (ChildNode->GetNodeObj() == NextState) {
					EndWidgetGeometry = &(ArrangedNodes[NodeIndex]);
				}
			}
		}
	}
	else
	{
		StartWidgetGeometry = PinGeometries->Find(OutputPinWidget);

		if (TSharedRef<SGraphPin>* pTargetWidget = PinToPinWidgetMap.Find(InputPin))
		{
			TSharedRef<SGraphPin> InputWidget = *pTargetWidget;
			EndWidgetGeometry = PinGeometries->Find(InputWidget);
		}
	}
}

void FFSMConnectionDrawingPolicy::DrawSplineWithArrow(const FGeometry & StartGeom, const FGeometry & EndGeom, const FConnectionParams & Params)
{	
	// Get a reasonable seed point (halfway between the boxes)
	const FVector2D StartCenter = FGeometryHelper::CenterOf(StartGeom);
	const FVector2D EndCenter = FGeometryHelper::CenterOf(EndGeom);
	const FVector2D SeedPoint = (StartCenter + EndCenter) * 0.5f;

	// Find the (approximate) closest points between the two boxes
	const FVector2D StartAnchorPoint = FGeometryHelper::FindClosestPointOnGeom(StartGeom, SeedPoint);
	const FVector2D EndAnchorPoint = FGeometryHelper::FindClosestPointOnGeom(EndGeom, SeedPoint);

	DrawSplineWithArrow(StartAnchorPoint, EndAnchorPoint, Params);
}

void FFSMConnectionDrawingPolicy::DrawSplineWithArrow(const FVector2D & StartPoint, const FVector2D & EndPoint, const FConnectionParams & Params)
{
	DrawLineWithArrowImpl(StartPoint, EndPoint, Params);

	// Is the connection bidirectional?
	/*if (Params.bUserFlag1)
	{
		DrawLineWithArrowImpl(EndAnchorPoint, StartAnchorPoint, Params);
	}*/
}

void FFSMConnectionDrawingPolicy::DrawPreviewConnector(const FGeometry & PinGeometry, const FVector2D & StartPoint, const FVector2D & EndPoint, UEdGraphPin * Pin)
{
	FConnectionParams Params;
	DetermineWiringStyle(Pin, nullptr, /*inout*/ Params);

	const FVector2D SeedPoint = EndPoint;
	const FVector2D AdjustedStartPoint = FGeometryHelper::FindClosestPointOnGeom(PinGeometry, SeedPoint);

	DrawSplineWithArrow(AdjustedStartPoint, EndPoint, Params);
}

FVector2D FFSMConnectionDrawingPolicy::ComputeSplineTangent(const FVector2D & Start, const FVector2D & End) const
{
	const FVector2D Delta = End - Start;
	const FVector2D NormDelta = Delta.GetSafeNormal();

	return NormDelta;
}

void FFSMConnectionDrawingPolicy::DrawLineWithArrowImpl(const FVector2D & StartAnchorPoint, const FVector2D & EndAnchorPoint, const FConnectionParams & Params)
{
	//@TODO: Should this be scaled by zoom factor?
	const float LineSeparationAmount = 4.5f;

	const FVector2D DeltaPos = EndAnchorPoint - StartAnchorPoint;
	const FVector2D UnitDelta = DeltaPos.GetSafeNormal();
	const FVector2D Normal = FVector2D(DeltaPos.Y, -DeltaPos.X).GetSafeNormal();

	// Come up with the final start/end points
	const FVector2D DirectionBias = Normal * LineSeparationAmount;
	const FVector2D LengthBias = ArrowRadius.X * UnitDelta;
	const FVector2D StartPoint = StartAnchorPoint + DirectionBias + LengthBias;
	const FVector2D EndPoint = EndAnchorPoint + DirectionBias - LengthBias;

	// Draw a line/spline
	DrawConnection(WireLayerID, StartPoint, EndPoint, Params);

	// Draw the arrow
	const FVector2D ArrowDrawPos = EndPoint - ArrowRadius;
	const float AngleInRadians = FMath::Atan2(DeltaPos.Y, DeltaPos.X);

	FSlateDrawElement::MakeRotatedBox(
		DrawElementsList,
		ArrowLayerID,
		FPaintGeometry(ArrowDrawPos, ArrowImage->ImageSize * ZoomFactor, ZoomFactor),
		ArrowImage,
		ESlateDrawEffect::None,
		AngleInRadians,
		TOptional<FVector2D>(),
		FSlateDrawElement::RelativeToElement,
		Params.WireColor
	);
}

FLinearColor FFSMConnectionDrawingPolicy::GetTransitionColor(bool bIsHovered)
{
	const FLinearColor HoverColor(0.724f, 0.256f, 0.0f, 1.0f);
	const FLinearColor BaseColor(0.9f, 0.5f, 0.6f, 1.0f);
	return bIsHovered ? HoverColor : BaseColor;
}
