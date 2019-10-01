#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "GraphEditor/NameValidators/IFSMNameValidator.h"

class FFSMTransitionNameValidator : public IFSMNameValidator
{
public:
	FFSMTransitionNameValidator(const class UFSMTransitionNode* InNode);

	virtual bool IsCurrentName(const FName& Name) const override;
	virtual bool IsExistingName(const FName& Name) const override;
};