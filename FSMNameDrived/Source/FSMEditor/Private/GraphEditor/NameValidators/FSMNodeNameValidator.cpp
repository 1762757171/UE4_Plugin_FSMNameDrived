#include "GraphEditor/NameValidators/FSMNodeNameValidator.h"
#include "FSM.h"
#include "GraphEditor/FSMGraph.h"
#include "GraphEditor/Nodes/FSMGraphNode.h"

FFSMNodeNameValidator::FFSMNodeNameValidator(const UFSMGraphNode * InNode) :
	IFSMNameValidator(InNode)
{}

bool FFSMNodeNameValidator::IsCurrentName(const FName & Name) const
{
	auto StateNode = Cast<UFSMGraphNode>(Node);
	return Name == StateNode->GetNodeName();
}

bool FFSMNodeNameValidator::IsExistingName(const FName & Name) const
{
	return FSM->HasState(Name);
}