// Fill out your copyright notice in the Description page of Project Settings.

#include "FSMComponent.h"
#include "FSM.h"

// Sets default values for this component's properties
UFSMComponent::UFSMComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UFSMComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	//Load it right now
	//it's small, no need to load async
	FSMAsset.LoadSynchronous();
}


// Called every frame
void UFSMComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UFSMComponent::RestartFSM()
{
	CurrentState = FSMAsset.Get()->EntryState;
	OnStateChanged.Broadcast(GetOwner(), NAME_None, CurrentState);
}

FName UFSMComponent::GetCurrentState() const
{
	return CurrentState;
}

void UFSMComponent::ReceiveKeyword(const FName & Condition)
{
	GotoStateDirectly(FSMAsset.Get()->MeetCondition(GetCurrentState(), Condition));
}

void UFSMComponent::GotoStateDirectly(const FName & NewState)
{
	if (!IsRunning()) {
		return;
	}
	auto NextState = NewState;
	if (!FSMAsset.Get()->HasState(NewState)) {
		NextState = NAME_None;
	}
	auto PrevState = GetCurrentState();
	CurrentState = NextState;
	OnStateChanged.Broadcast(GetOwner(), PrevState, CurrentState);
}

void UFSMComponent::EndFSM()
{
	GotoStateDirectly(NAME_None);
}

bool UFSMComponent::IsRunning() const
{
	return !(CurrentState.IsNone());
}

