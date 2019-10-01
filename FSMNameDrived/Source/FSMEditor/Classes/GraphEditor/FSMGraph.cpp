#include "FSMGraph.h"
#include "FSM.h"

UFSM * UFSMGraph::GetFSM() const
{
	//return CastChecked<UFSM>(GetOuter());
	return GetTypedOuter<UFSM>();
}

bool UFSMGraph::Modify(bool bAlwaysMarkDirty)
{
	if (!CanModify()) {
		return false;
	}
	bool bSaved = Super::Modify(bAlwaysMarkDirty);
	return bSaved;
}
