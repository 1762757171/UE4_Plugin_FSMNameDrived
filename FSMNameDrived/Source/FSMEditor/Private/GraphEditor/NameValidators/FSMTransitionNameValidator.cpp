#include "GraphEditor/NameValidators/FSMTransitionNameValidator.h"
#include "FSM.h"
#include "GraphEditor/FSMGraph.h"
#include "GraphEditor/Connection/FSMTransitionNode.h"
#include "GraphEditor/Nodes/FSMGraphNode.h"

FFSMTransitionNameValidator::FFSMTransitionNameValidator(const UFSMTransitionNode * InNode) :
	IFSMNameValidator(InNode)
{}

bool FFSMTransitionNameValidator::IsCurrentName(const FName& Name) const
{
	auto TransitionNode = Cast<UFSMTransitionNode>(Node);
	return Name == TransitionNode->GetTransitionName();
}

bool FFSMTransitionNameValidator::IsExistingName(const FName & Name) const
{
	auto TransitionNode = Cast<UFSMTransitionNode>(Node);
	return FSM->HasRule(TransitionNode->GetFromNode()->GetNodeName(), Name);
}