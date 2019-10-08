#pragma once
// Fill out your copyright notice in the Description page of Project Settings.

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FSM.generated.h"

USTRUCT()
struct FSMASSET_API FTransRule
{
	GENERATED_BODY()
public:
	UPROPERTY()
	TMap<FName, FName> Keyword_mapping_TargetState;
	UPROPERTY()
	FName FromState;
public:
	//return Target Node Name
	FName MeetCondition(const FName& RuleName) const;

	bool IsValidRule(const FName& RuleName) const;

	void AddRule(const FName& RuleName, const FName& DestState);

	void ChangeSourceState(const FName& FS);

	bool DeleteState(const FName& StateName);
	bool DeleteRule(const FName& RuleName);
	bool RenameRule(const FName& OldName, const FName& NewName);
	bool RenameState(const FName& OldName, const FName& NewName);
};

USTRUCT()
struct FSMASSET_API FFSMData
{
	GENERATED_BODY()
public:
	UPROPERTY()
	TMap<FName, FTransRule> CurrState_Mapping_TransRule;
public:
	//return Target Node Name
	FName MeetCondition(const FName& CurrentState, const FName& RuleName) const;

	bool IsValidState(const FName& State) const;

	bool AddRule(const FName& RuleName, const FName& FromState, const FName& DestState);

	bool AddState(const FName& NewState);

	bool RenameState(const FName& OldName, const FName& NewName);
	bool RenameRule(const FName& FromState, const FName& OldName, const FName& NewName);

	bool DeleteState(const FName& StateName);
	bool DeleteRule(const FName& FromState, const FName& RuleName);
};

/**
 * 
 */
UCLASS(hidecategories = Object, BlueprintType)
class FSMASSET_API UFSM : public UObject
{
	GENERATED_BODY()
	
public:
	bool HasState(const FName& StateName) const;
	bool HasRule(const FName& FromState, const FName& RuleName) const;

	//void Tick(float DeltaSecond);

	FName MeetCondition(const FName& CurrentState, const FName& RuleName);
	
	bool AddRule(const FName& RuleName, const FName& FromState, const FName& DestState);

	bool AddState(const FName& NewState);

	bool RenameState(const FName& OldName, const FName& NewName);
	bool RenameRule(const FName& FromState, const FName& OldName, const FName& NewName);

	bool DeleteState(const FName& StateName);
	bool DeleteRule(const FName& FromState, const FName& RuleName);

	void SetEntryState(const FName& NewEntry);
	FName GetEntryState() const;
public:
	UPROPERTY()
	class UEdGraph* GraphView;

	UPROPERTY()
		FName EntryState;

private:
	UPROPERTY()
	FFSMData StateMachine;
};
