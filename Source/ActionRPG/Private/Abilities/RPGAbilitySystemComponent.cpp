#include "RPGAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "RPGCharacterBase.h"

void URPGAbilitySystemComponent::GetActiveAbilitiesWithTags(const FGameplayTagContainer& TagContainer, TArray<URPGGameplayAbility*>& ActiveAbilities) {
	TArray<FGameplayAbilitySpec*> AbilitySpecs;
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(TagContainer, AbilitySpecs, false);
	for (FGameplayAbilitySpec* Spec : AbilitySpecs) {
		TArray<UGameplayAbility*> Abilities = Spec->GetAbilityInstances();
		for(UGameplayAbility* Ability: Abilities) {
			ActiveAbilities.Add(Cast<URPGGameplayAbility>(Ability));
		}
	}
}

int32 URPGAbilitySystemComponent::GetDefaultAbilityLevel() const
{
	ARPGCharacterBase* Character = Cast<ARPGCharacterBase>(OwnerActor);
	if (Character) {
		return Character->GetCharacterLevel();
	}
	return 1;
}

URPGAbilitySystemComponent* URPGAbilitySystemComponent::GetAbilitySystemComponentFromActor(const AActor* Actor, bool LookForComponent) {
	
	return Cast<URPGAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor, LookForComponent));
}