#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"

#include "FSMGraph.generated.h"

UCLASS()
class UFSMGraph : public UEdGraph
{
	GENERATED_BODY()
public:
	/** Gets the DlgDialogue that contains this graph */
	class UFSM* GetFSM() const;

	/**
	 * Note that the object will be modified. If we are currently recording into the
	 * transaction buffer (undo/redo), save a copy of this object into the buffer and
	 * marks the package as needing to be saved.
	 *
	 * @param	bAlwaysMarkDirty	if true, marks the package dirty even if we aren't
	 *								currently recording an active undo/redo transaction
	 * @return true if the object was saved to the transaction buffer
	 */
	virtual bool Modify(bool bAlwaysMarkDirty = true) override;

public:
	UPROPERTY()
		class UFSMGraphEntryNode* Entry;
};