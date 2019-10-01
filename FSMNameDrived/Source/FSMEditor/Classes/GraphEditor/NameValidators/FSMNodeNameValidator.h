#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "GraphEditor/NameValidators/IFSMNameValidator.h"

class FFSMNodeNameValidator : public IFSMNameValidator
{
public:
	FFSMNodeNameValidator(const class UFSMGraphNode* InNode);

	virtual bool IsCurrentName(const FName& Name) const override;
	virtual bool IsExistingName(const FName& Name) const override;
};