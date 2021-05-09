#pragma once

/**
* 在这里我们将定义一些获取目标的类，在动作游戏中，
* 有些能力的目标是玩家角色本身，例如吃一些加血的药品
* 有些能力的目标则是碰撞到的东西，例如地方小怪，而在这里我们对这些目标已经获取的方法进行定义。
*/

#include "../ActionRPG.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "RPGTargetType.generated.h"

class ARPGCharacterBase;

/**
* 一个目标类的基类，此类的GetTargets 方法默认为空实现
*/
UCLASS(Blueprintable, meta=(ShowWorldContextPin))
class ACTIONRPG_API URPGTargetType: public UObject {
	GENERATED_BODY()
public:
	// Construct
	URPGTargetType() {};

	/**
	* 获取目标
	* @param InstigatorCharacter 能力的发起角色
	* @param TargetActor 能力发起者的 Actor, 例如火球
	* @param EventData GamePlay的事件数据，记录一些外部的碰撞，例如一个火球或者武器碰到一个地方小怪，小怪的数据和碰撞的数据救会记录在当中
	* @param OutHitResults 需要返回的碰撞结果
	* @param OutActors 需要返回的目标Actor
	*/
	UFUNCTION(BlueprintNativeEvent)
		void GetTargets(
			ARPGCharacterBase* InstigatorCharacter,
			AActor* InstigatorActor,
			FGameplayEventData EventData,
			TArray<FHitResult>& OutHitResults,
			TArray<AActor*>& OutActors
		) const;

};

/**
* 使用自己作为目标对象的基类，例如服用药品等能力，就可以使用此对象
*/
UCLASS(Blueprintable, meta=(ShowWorldContextPin))
class ACTIONRPG_API URPGTargetType_UseOwner : public URPGTargetType {
	GENERATED_BODY()
public:
	URPGTargetType_UseOwner();

	virtual void GetTargets_Implementation(
		ARPGCharacterBase* InstigatorCharacter,
		AActor* InstigatorActor,
		FGameplayEventData EventData,
		TArray<FHitResult>& OutHitResults,
		TArray<AActor*>& OutActors
	) const override;
};

/**
* 使用GameplayEventData 来获取目标，例如挥剑造成的外部碰撞事件
*/
UCLASS(Blueprintable, meta=(ShowWorldContextPin))
class ACTIONRPG_API URPGTargetType_UseEventData : public URPGTargetType {
	GENERATED_BODY()
public:
	URPGTargetType_UseEventData();

	virtual void GetTargets_Implementation(
		ARPGCharacterBase* InstigatorCharacter,
		AActor* InstigatorActor,
		FGameplayEventData EventData,
		TArray<FHitResult>& OutHitResults,
		TArray<AActor*>& OutActors
	) const override;
};