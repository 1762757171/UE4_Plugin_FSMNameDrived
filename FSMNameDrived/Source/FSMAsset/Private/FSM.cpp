// Fill out your copyright notice in the Description page of Project Settings.

#include "FSM.h"

FName FTransRule::MeetCondition(const FName& RuleName) const
{
	if (!IsValidRule(RuleName)) {
		return NAME_None;
	}
	return Keyword_mapping_TargetState[RuleName];
}

bool FTransRule::IsValidRule(const FName & RuleName) const
{
	return Keyword_mapping_TargetState.Contains(RuleName);
}

void FTransRule::AddRule(const FName& RuleName, const FName& DestState)
{
	Keyword_mapping_TargetState.Add(RuleName, DestState);
}

void FTransRule::ChangeSourceState(const FName& FS)
{
	FromState = FS;
}

bool FTransRule::DeleteState(const FName& StateName)
{
	check(FromState != StateName);

	//use keys instead of iterator
	//because remove() action will destroy iterator
	TArray<FName> Keys;
	int32 KeyNum = Keyword_mapping_TargetState.GetKeys(Keys);
	for (int32 i = 0; i < KeyNum; ++i) {
		if (Keyword_mapping_TargetState[Keys[i]] == StateName) {
			Keyword_mapping_TargetState.Remove(Keys[i]);
		}
	}
	return true;
}

bool FTransRule::DeleteRule(const FName& RuleName)
{
	if (!IsValidRule(RuleName)) {
		return false;
	}
	Keyword_mapping_TargetState.Remove(RuleName);
	return true;
}

bool FTransRule::RenameRule(const FName& OldName, const FName& NewName)
{
	if (!IsValidRule(OldName)) {
		return false;
	}
	if (IsValidRule(NewName)) {
		return false;
	}
	FName temp;
	Keyword_mapping_TargetState.RemoveAndCopyValue(OldName, temp);
	Keyword_mapping_TargetState.Add(NewName, temp);
	return true;
}

bool FTransRule::RenameState(const FName& OldName, const FName& NewName)
{
	if ((!IsValidRule(OldName)) && (FromState != OldName)) {
		return false;
	}
	if (FromState == OldName) {
		FromState = NewName;
	}
	for (auto it = Keyword_mapping_TargetState.CreateIterator(); it; ++it) {
		if (it.Value() == OldName) {
			it.Value() = NewName;
		}
	}
	return true;
}

FName FFSMData::MeetCondition(const FName& CurrentState, const FName& RuleName) const
{
	if (!IsValidState(CurrentState)) {
		return NAME_None;
	}
	auto transRule = CurrState_Mapping_TransRule[CurrentState];
	auto targetState = transRule.MeetCondition(RuleName);
	return targetState;
}

bool FFSMData::IsValidState(const FName& State) const
{
	return CurrState_Mapping_TransRule.Contains(State);
}

bool FFSMData::AddRule(const FName& RuleName, const FName& FromState, const FName& DestState)
{
	if (!IsValidState(FromState)) {
		return false;
	}
	FTransRule& Rule = CurrState_Mapping_TransRule[FromState];
	Rule.ChangeSourceState(FromState);
	Rule.AddRule(RuleName, DestState);

	return true;
}

bool FFSMData::AddState(const FName& NewState)
{
	if (IsValidState(NewState)) {
		return false;
	}

	FTransRule NewRule;
	NewRule.ChangeSourceState(NewState);

	CurrState_Mapping_TransRule.Add(NewState, NewRule);
	return true;
}

bool FFSMData::RenameState(const FName& OldName, const FName& NewName)
{
	if (!IsValidState(OldName)) {
		return false;
	}
	if (IsValidState(NewName)) {
		return false;
	}
	FTransRule tempTR;
	CurrState_Mapping_TransRule.RemoveAndCopyValue(OldName, tempTR);
	CurrState_Mapping_TransRule.Add(NewName, tempTR);
	for (auto it = CurrState_Mapping_TransRule.CreateIterator(); it; ++it) {
		it.Value().RenameState(OldName, NewName);
	}
	return true;
}

bool FFSMData::RenameRule(const FName& FromState, const FName& OldName, const FName& NewName)
{
	if (!IsValidState(FromState)) {
		return false;
	}
	return CurrState_Mapping_TransRule[FromState].RenameRule(OldName, NewName);
}

bool FFSMData::DeleteState(const FName& StateName)
{
	if (!IsValidState(StateName)) {
		return false;
	}
	CurrState_Mapping_TransRule.Remove(StateName);
	for (auto it = CurrState_Mapping_TransRule.CreateIterator(); it; ++it) {
		it.Value().DeleteState(StateName);
	}
	return true;
}

bool FFSMData::DeleteRule(const FName& FromState, const FName& RuleName)
{
	if (!IsValidState(FromState)) {
		return false;
	}
	return CurrState_Mapping_TransRule[FromState].DeleteRule(RuleName);
}

bool UFSM::HasState(const FName & StateName) const
{
	return StateMachine.IsValidState(StateName);
}

bool UFSM::HasRule(const FName & FromState, const FName & RuleName) const
{
	if (!HasState(FromState)) {
		return false;
	}
	return StateMachine.CurrState_Mapping_TransRule[FromState].IsValidRule(RuleName);
}

FName UFSM::MeetCondition(const FName& CurrentState, const FName& RuleName)
{
	return StateMachine.MeetCondition(CurrentState, RuleName);
}

bool UFSM::AddRule(const FName& RuleName, const FName& FromState, const FName& DestState)
{
	return StateMachine.AddRule(RuleName, FromState, DestState);
}

bool UFSM::AddState(const FName& NewState)
{
	return StateMachine.AddState(NewState);
}

bool UFSM::RenameState(const FName& OldName, const FName& NewName)
{
	if (OldName == NewName) {
		return false;
	}
	return StateMachine.RenameState(OldName, NewName);
}

bool UFSM::RenameRule(const FName& FromState, const FName& OldName, const FName& NewName)
{
	if (OldName == NewName) {
		return false;
	}
	return StateMachine.RenameRule(FromState, OldName, NewName);
}

bool UFSM::DeleteState(const FName& StateName)
{
	return StateMachine.DeleteState(StateName);
}

bool UFSM::DeleteRule(const FName& FromState, const FName& RuleName)
{
	return StateMachine.DeleteRule(FromState, RuleName);
}

void UFSM::SetEntryState(const FName & NewEntry)
{
	EntryState = NewEntry;
}

FName UFSM::GetEntryState() const
{
	return EntryState;
}
