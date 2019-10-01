#include "FSMGraphPin.h"
#include "Widgets/Layout/SSpacer.h"

//Reference: D:\Unreal Engine\UE_4.21\Engine\Source\Editor\AnimationBlueprintEditor\Private\AnimationStateNodes\SGraphNodeAnimState.cpp

void SFSMGraphPin::Construct(const FArguments & InArgs, UEdGraphPin * InPin)
{
	this->SetCursor(EMouseCursor::Default);

	bShowLabel = true;

	GraphPinObj = InPin;
	check(GraphPinObj != NULL);

	const UEdGraphSchema* Schema = GraphPinObj->GetSchema();
	check(Schema);

	CachedImg_Pin_Background = FEditorStyle::GetBrush(TEXT("Graph.StateNode.Pin.Background"));
	CachedImg_Pin_BackgroundHovered = FEditorStyle::GetBrush(TEXT("Graph.StateNode.Pin.BackgroundHovered"));

	// Set up a hover for pins that is tinted the color of the pin.
	SBorder::Construct(SBorder::FArguments()
		.BorderImage(this, &SFSMGraphPin::GetPinBorder)
		.BorderBackgroundColor(this, &SFSMGraphPin::GetPinColor)
		.OnMouseButtonDown(this, &SFSMGraphPin::OnPinMouseDown)
		.Cursor(this, &SFSMGraphPin::GetPinCursor)
	);
}

TSharedRef<SWidget> SFSMGraphPin::GetDefaultValueWidget()
{
	return SNew(STextBlock);
}