#pragma once

/**
* 能力系统组件
*/
#include "GameplayAbilities/Public/AbilitySystemComponent.h"
#include "RPGGameplayAbility.h"
#include "RPGAbilitySystemComponent.generated.h"

UCLASS()
class ACTIONRPG_API URPGAbilitySystemComponent : public UAbilitySystemComponent {
	GENERATED_BODY()
public:

	/**
	* 根据标签组返回可以激活的能力
	* @param TagContainer 标签组
	* @param ActiveAbilities 返回的可激活能力数组
	*/
	void GetActiveAbilitiesWithTags(const FGameplayTagContainer& TagContainer, TArray<URPGGameplayAbility*>& ActiveAbilities);

	/**
	* 获取默认的能力
	*/
	int32 GetDefaultAbilityLevel() const;
	
	/**
	* 根据Actor去查找Actor所关联的AbilitySystemComponent
	* 一般我们要求传递来的Actor参数需要实现 IAbilitySystemInterface 接口来方便查询
	* @param Actor 根据此 Actor 来检索 AbilitySystemComponet 组件
	* @param LookForComponent 是否调用 Actor自身中FindComponentByClass来查找 AbilitySystemCompent,一般默认为 false, 因此此方法低效而且结果总是不理想的
	* @return 如果 找到返回组件指针，否则返回空指针
	*/
	static URPGAbilitySystemComponent* GetAbilitySystemComponentFromActor(const AActor* Actor, bool LookForComponent = false);
};