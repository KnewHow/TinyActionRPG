#pragma once
/**
* ARPG GameAblity 基类
*/
#include "Abilities/GameplayAbility.h"
#include "RPGAbilityTypes.h"
#include "RPGGameplayAbility.generated.h"

UCLASS()
class ACTIONRPG_API URPGGameplayAbility : public UGameplayAbility {
	GENERATED_BODY()
public:

	URPGGameplayAbility();

	/**
	* 标签和游戏效果(GameplayEffect)的映射，这里可以配置不同种类攻击的伤害类型
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = GameplayEffects)
		TMap<FGameplayTag, FRPGGameplayEffectContainer> EffectContainerMap;

	/**
	* 将原始的FRPGGameplayEffectContainer 转换为 FRPGGameplayEffectContainerSpec
	* 然后被GameplayAbility处理
	* @param Container 原始的FRPGGameplayEffectContainer
	* @param EventData 事件数据，里面包含了TargetType和HitResults
	* @param OverrideGameplayLevel
	* @param OverrideGameplayLevel 重载的能力等级，好像没啥用，使用默认的即可
	*/
	UFUNCTION(BlueprintCallable, Category = Ability, meta = (AutoCreateRefTerm = "EventData"))
		virtual FRPGGameplayEffectContainerSpec MakeEffectContainerSpecFromContainer(
			const FRPGGameplayEffectContainer& Container,
			const FGameplayEventData& EventData,
			int32 OverrideGameplayLevel = -1);

	/**
	* 根据ContainerTag和EventData来创建FRPGGameplayEffectContainerSpec
	* 我们只有从此类的属性EffectContainerMap中根据ContainerTag获取对应的FRPGGameplayEffectContainer
	* 然后调用MakeEffectContainerSpecFromContainer 即可
	* @param ContainerTag 容器标签，用来获取FRPGGameplayEffectContainer
	* @param EventData 用来获取 TargetType 和 HitResult
	* @param 重载的能力等级，不知道有什么用
	*/
	UFUNCTION(BlueprintCallable, Category = Ability, meta = (AutoCreateRefTerm = "EventData"))
		virtual FRPGGameplayEffectContainerSpec MakeEffectContainerSpec(
			const FGameplayTag& ContainerTag,
			const FGameplayEventData& EventData,
			int32 OverrideGameplayLevel = -1
		);

	
	/**
	* 将FRPGGameplayEffectContainerSpec应用到目标上
	* @param ContainerSpec 被转换之后的 FRPGGameplayEffectContainerSpec
	*/
	UFUNCTION(BlueprintCallable, Category=Ability)
	virtual TArray<FActiveGameplayEffectHandle> ApplyEffectContainerSpec(
		const FRPGGameplayEffectContainerSpec& ContainerSpec
	);

	/**
	* 根据标签和事件数据来将GameplayEffect应用到目标上
	* @param ContainerTag 容器标签，用来获取FRPGGameplayEffectContainer
	* @param EventData 用来获取 TargetType 和 HitResult
	* @param 重载的能力等级，不知道有什么用
	*/
	UFUNCTION(BlueprintCallable, Category = Ability, meta = (AutoCreateRefTerm = "EventData"))
		virtual TArray<FActiveGameplayEffectHandle> ApplyEffectContainer(
			const FGameplayTag& ContainerTag,
			const FGameplayEventData& EventData,
			int32 OverrideGameplayLevel = -1
		);

	UFUNCTION(BlueprintCallable)
	FString DebugGetCharacterName();

};