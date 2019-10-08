// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FSMComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FStateChangedSignature, AActor*, Owner, FName, FromState, FName, DestState);

UCLASS(Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent) )
class FSMASSET_API UFSMComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFSMComponent();

public:	
	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UFUNCTION(BlueprintCallable)
	void ChangeFSM(class UFSM* NewFSM);

	//Restart FSM, with empty previous state
	UFUNCTION(BlueprintCallable)
	void RestartFSM();

	UFUNCTION(BlueprintCallable)
	FName GetCurrentState() const;

	UFUNCTION(BlueprintCallable)
	void ReceiveKeyword(const FName& Condition);

	UFUNCTION(BlueprintCallable)
	void GotoStateDirectly(const FName& NewState);

	UFUNCTION(BlueprintCallable)
	void EndFSM();

	UFUNCTION(BlueprintCallable)
	bool IsRunning() const;

private:
	UPROPERTY(EditDefaultsOnly)
		TAssetPtr<class UFSM> FSMAssetPtr;

	UPROPERTY(VisibleInstanceOnly)
	class UFSM* FSM;

	UPROPERTY(VisibleAnywhere)
		FName CurrentState;

	UPROPERTY(BlueprintAssignable)
		FStateChangedSignature OnStateChanged;
};
