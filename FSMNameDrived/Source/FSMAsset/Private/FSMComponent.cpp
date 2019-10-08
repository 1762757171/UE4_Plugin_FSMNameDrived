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
	//Load it right now
	//It's not allow to load async
	if (FSMAssetPtr.IsNull()) {
		return;
	}
	FSM = FSMAssetPtr.LoadSynchronous();

	Super::BeginPlay();
}


// Called every frame
void UFSMComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UFSMComponent::ChangeFSM(class UFSM* NewFSM)
{
	FSM = NewFSM;
	EndFSM();
}

void UFSMComponent::RestartFSM()
{
	if (!FSM) {
		EndFSM();
		return;
	}
	CurrentState = FSM->EntryState;
	OnStateChanged.Broadcast(GetOwner(), NAME_None, CurrentState);
}

FName UFSMComponent::GetCurrentState() const
{
	return CurrentState;
}

void UFSMComponent::ReceiveKeyword(const FName & Condition)
{
	GotoStateDirectly(FSM->MeetCondition(GetCurrentState(), Condition));
}

void UFSMComponent::GotoStateDirectly(const FName & NewState)
{
	if (!IsRunning()) {
		return;
	}
	auto NextState = NewState;
	if (!FSM->HasState(NewState)) {
		return;
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
	return (!FSM) || (!(CurrentState.IsNone()));
}

