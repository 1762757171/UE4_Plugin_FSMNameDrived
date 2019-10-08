#include "GraphEditor/NameValidators/IFSMNameValidator.h"
#include "FSM.h"
#include "GraphEditor/FSMGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "GraphEditor/Connection/FSMTransitionNode.h"
#include "GraphEditor/Nodes/FSMGraphNode.h"

IFSMNameValidator::IFSMNameValidator(const UEdGraphNode * InNode) :
	Node(InNode)
{
	auto Graph = Cast<UFSMGraph>(InNode->GetOuter());
	FSM = Graph->GetFSM();
}

EValidatorResult IFSMNameValidator::IsValid(const FString & Name, bool bOriginal)
{
	return IsValid(FName(*Name), bOriginal);
}

EValidatorResult IFSMNameValidator::IsValid(const FName & Name, bool bOriginal)
{
	if (Name.IsNone()) {
		return EValidatorResult::EmptyName;
	}
	if (!Name.IsValidXName()) {
		return EValidatorResult::ContainsInvalidCharacters;
	}
	if (IsCurrentName(Name)) {
		return EValidatorResult::ExistingName;
	}
	if (IsExistingName(Name)) {
		return EValidatorResult::AlreadyInUse;
	}
	return EValidatorResult::Ok;
}
