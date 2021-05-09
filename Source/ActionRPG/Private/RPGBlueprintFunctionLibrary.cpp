#include "RPGBlueprintFunctionLibrary.h"

URPGBlueprintFunctionLibrary::URPGBlueprintFunctionLibrary(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer) {
}

FRPGGameplayEffectContainerSpec URPGBlueprintFunctionLibrary::AddTargetsToEffectContainerSpec(const FRPGGameplayEffectContainerSpec& Spec, const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors) {
	FRPGGameplayEffectContainerSpec NewSpec = Spec;
	NewSpec.AddTargets(HitResults, TargetActors);
	return NewSpec;
}

TArray<FActiveGameplayEffectHandle> URPGBlueprintFunctionLibrary::ApplyExternalEffectContainerSpec(const FRPGGameplayEffectContainerSpec& Spec)
{
	TArray<FActiveGameplayEffectHandle> AllEffects;
	for (const FGameplayEffectSpecHandle& Handle : Spec.TargetGameplayEffectSpecs) {
		if (Handle.IsValid()) {
			for (TSharedPtr<FGameplayAbilityTargetData> Data : Spec.TargetData.Data) {
				AllEffects.Append(Data->ApplyGameplayEffectSpec(*Handle.Data.Get()));
			}
		}
	}
	return AllEffects;
}

bool URPGBlueprintFunctionLibrary::DoesEffectContainerSpecHaveEffects(const FRPGGameplayEffectContainerSpec& Spec)
{
	return Spec.HasValidEffects();
}
