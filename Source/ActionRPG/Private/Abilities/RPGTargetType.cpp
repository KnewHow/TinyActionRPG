#include "RPGTargetType.h"
#include "RPGCharacterBase.h"

void URPGTargetType::GetTargets_Implementation(ARPGCharacterBase* InstigatorCharacter, AActor* InstigatorActor, FGameplayEventData EventData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const
{
	return;
}

URPGTargetType_UseOwner::URPGTargetType_UseOwner() {}

void URPGTargetType_UseOwner::GetTargets_Implementation(
	ARPGCharacterBase* InstigatorCharacter,
	AActor* InstigatorActor,
	FGameplayEventData EventData,
	TArray<FHitResult>& OutHitResults,
	TArray<AActor*>& OutActors
)const {
	OutActors.Add(InstigatorActor);
}

URPGTargetType_UseEventData::URPGTargetType_UseEventData() {}

void URPGTargetType_UseEventData::GetTargets_Implementation(
	ARPGCharacterBase* InstigatorCharacter,
	AActor* InstigatorActor,
	FGameplayEventData EventData,
	TArray<FHitResult>& OutHitResults,
	TArray<AActor*>& OutActors
)const {
	const FHitResult* FoundHitResult = EventData.ContextHandle.GetHitResult();

	if (FoundHitResult) {
		OutHitResults.Add(*FoundHitResult);
	}
	else if(EventData.Target){
		OutActors.Add(const_cast<AActor*>(EventData.Target));
	}
}
