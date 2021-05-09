#include "RPGGameplayAbility.h"
#include "RPGCharacterBase.h"
#include "RPGAbilitySystemComponent.h"

URPGGameplayAbility::URPGGameplayAbility() {}

FRPGGameplayEffectContainerSpec URPGGameplayAbility::MakeEffectContainerSpecFromContainer(const FRPGGameplayEffectContainer& Container, const FGameplayEventData& EventData, int32 OverrideGameplayLevel)
{
	FRPGGameplayEffectContainerSpec Res;
	AActor* OwningActor = GetOwningActorFromActorInfo();
	ARPGCharacterBase* Character = Cast<ARPGCharacterBase>(OwningActor);
	URPGAbilitySystemComponent* OwningASC = URPGAbilitySystemComponent::GetAbilitySystemComponentFromActor(Character);
	if (OwningASC) {
		if (Container.TargetType.Get()) {
			TArray<FHitResult> HitResults;
			TArray<AActor*> TargetActors;
			const URPGTargetType* TargetTypeInstance = Container.TargetType.GetDefaultObject();
			TargetTypeInstance->GetTargets(Character, OwningActor, EventData, HitResults, TargetActors);
			Res.AddTargets(HitResults, TargetActors);
		}

		if (OverrideGameplayLevel == INDEX_NONE) {
			OverrideGameplayLevel = this->GetAbilityLevel();
		}

		for (const TSubclassOf<UGameplayEffect> EffectClass : Container.TargetGameplayEffectClasses) {
			Res.TargetGameplayEffectSpecs.Add(MakeOutgoingGameplayEffectSpec(EffectClass, OverrideGameplayLevel));;
		}
	}
	return Res;
}

FRPGGameplayEffectContainerSpec URPGGameplayAbility::MakeEffectContainerSpec(const FGameplayTag& ContainerTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel)
{
	FRPGGameplayEffectContainer* EffectContainer = EffectContainerMap.Find(ContainerTag);
	if (EffectContainer) {
		return MakeEffectContainerSpecFromContainer(*EffectContainer, EventData, OverrideGameplayLevel);;
	}
	return FRPGGameplayEffectContainerSpec();
}

TArray<FActiveGameplayEffectHandle> URPGGameplayAbility::ApplyEffectContainerSpec(const FRPGGameplayEffectContainerSpec& ContainerSpec)
{
	TArray<FActiveGameplayEffectHandle> AllEffects;
	for (const FGameplayEffectSpecHandle& Handle : ContainerSpec.TargetGameplayEffectSpecs) {
		AllEffects.Append(K2_ApplyGameplayEffectSpecToTarget(Handle, ContainerSpec.TargetData));
	}
	return AllEffects;
}

TArray<FActiveGameplayEffectHandle> URPGGameplayAbility::ApplyEffectContainer(const FGameplayTag& ContainerTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel)
{
	auto ContainerSpec = MakeEffectContainerSpec(ContainerTag, EventData, OverrideGameplayLevel);
	return ApplyEffectContainerSpec(ContainerSpec);
}

FString URPGGameplayAbility::DebugGetCharacterName()
{
	AActor* OwningActor = GetOwningActorFromActorInfo();
	ARPGCharacterBase* Character = Cast<ARPGCharacterBase>(OwningActor);
	if (Character) {
		return Character->GetName();
	}
	else {
		return FString("GetCharactor Failure!");
	}
	
}
