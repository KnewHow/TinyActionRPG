#pragma once

/**
* �����˺����ⲿ��
*/
#include "ActionRPG.h"
#include "GameplayEffect.h"
#include "GameplayEffectExecutionCalculation.h"
#include "RPGDamageExecution.generated.h"

UCLASS()
class ACTIONRPG_API URPGDamageExecution : public UGameplayEffectExecutionCalculation {
	GENERATED_BODY()
public:
	URPGDamageExecution();
	
	void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};