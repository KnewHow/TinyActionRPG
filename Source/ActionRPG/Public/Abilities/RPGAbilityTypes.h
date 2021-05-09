#pragma once

/**
* 这里，我们将会定义一些 GameplayEffect和 URPGTarget的一些关系。
* 简单来说就是：目标和作用效果的对应关系。
* 在第一个结构体中，我们定义和本工程相关的Target和GameplayEffect的关系，稍后你可以在蓝图中配置他们
* 在第二个结构体中，我们定义GameplayAbility系统中需要的Target和GameplayEffect，这些是最终被Ability框架所执行的东西
* 最后我们会在[RPGGameplayAbility]中实现二者间的转化以及应用
* 
*/
#include "ActionRPG.h"
#include "RPGTargetType.h"
#include "GameplayAbilities/Public/GameplayEffect.h"
#include "GameplayAbilities/Public/Abilities/GameplayAbilityTargetTypes.h"
#include "RPGAbilityTypes.generated.h"

USTRUCT(BlueprintType)
struct FRPGGameplayEffectContainer {
	GENERATED_BODY()
public:
	FRPGGameplayEffectContainer();

	/**
	* 目标类型
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
		TSubclassOf<URPGTargetType> TargetType;

	/**
	* 目标对应的效果集
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
		TArray<TSubclassOf<UGameplayEffect>> TargetGameplayEffectClasses;
};

/**
* 这是一个被处理的版本关于上面的FRPGGameplayEffectContainer
* 我们将会把上面的类型通过一些运算，转换给该类型，最后交给GamepalyAbility处理
* 
*/
USTRUCT(BlueprintType)
struct FRPGGameplayEffectContainerSpec {
	GENERATED_BODY()
public:
	FRPGGameplayEffectContainerSpec();

	/**
	* 目标类型
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
		FGameplayAbilityTargetDataHandle TargetData;

	/**
	* 作用在目标身上的GameplayEffect
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
		TArray<FGameplayEffectSpecHandle> TargetGameplayEffectSpecs;
	

	/**
	* 是否有合法的目标
	*/
	bool hasValidTargets() const;

	/**
	* 是否有合法的效果
	*/
	bool HasValidEffects() const;

	/**
	* 添加目标
	* @param HitResults 碰撞结果
	* @param TargetActors 目标Actor
	*/
	void AddTargets(const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors);
};