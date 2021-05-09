#include "RPGAbilityTypes.h"

FRPGGameplayEffectContainer::FRPGGameplayEffectContainer() {}

FRPGGameplayEffectContainerSpec::FRPGGameplayEffectContainerSpec() {}

bool FRPGGameplayEffectContainerSpec::hasValidTargets() const
{
	return TargetData.Num() > 0;
}

bool FRPGGameplayEffectContainerSpec::HasValidEffects() const
{
	return TargetGameplayEffectSpecs.Num() > 0;
}

void FRPGGameplayEffectContainerSpec::AddTargets(
	const TArray<FHitResult>& HitResults, 
	const TArray<AActor*>& TargetActors)
{
	for (const FHitResult& HitResult : HitResults) {
		FGameplayAbilityTargetData_SingleTargetHit* NewData = new  FGameplayAbilityTargetData_SingleTargetHit(HitResult);
		TargetData.Add(NewData);
	}

	if (TargetActors.Num() > 0) {
		FGameplayAbilityTargetData_ActorArray* newData = new FGameplayAbilityTargetData_ActorArray();
		newData->TargetActorArray.Append(TargetActors);
		TargetData.Add(newData);
	}
}
