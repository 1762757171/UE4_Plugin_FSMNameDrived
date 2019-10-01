#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Kismet2NameValidators.h"

class IFSMNameValidator : public INameValidatorInterface
{
public:
	IFSMNameValidator(const class UEdGraphNode* InNode);
	virtual EValidatorResult IsValid(const FString& Name, bool bOriginal) override;
	virtual EValidatorResult IsValid(const FName& Name, bool bOriginal) override;

	virtual bool IsCurrentName(const FName& Name) const = 0;
	virtual bool IsExistingName(const FName& Name) const = 0;
protected:
	const class UEdGraphNode* Node;
	class UFSM* FSM;
};