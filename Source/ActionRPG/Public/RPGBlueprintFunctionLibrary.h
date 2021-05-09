#pragma once
#include "ActionRPG.h"
#include "Abilities/RPGAbilityTypes.h"
#include "RPGBlueprintFunctionLibrary.generated.h"

/**
* C++的一个通用蓝图函数库
*/

UCLASS()
class URPGBlueprintFunctionLibrary : public UBlueprintFunctionLibrary {
	GENERATED_UCLASS_BODY()
public:
	/**
	* 给EffectContainerSpec 添加目标，和
	* @param Spec 需要添加目标的 EffectContainerSpec
	* @param HitResults 碰撞结果
	* @param TargetActors 目标Actor
	* @return 返回新的 EffectContainerSpec
	*/
	UFUNCTION(BlueprintCallable, Category = Ability, meta = (AutoCreateRefTerm = "HitResults,TargetActors"))
		static FRPGGameplayEffectContainerSpec AddTargetsToEffectContainerSpec(const FRPGGameplayEffectContainerSpec& Spec, const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors);

	/**
	* 将外部的EffectContainerSpec 应用的目标上，用来完成应用伤害等功能
	* @param Spec 外部的EffectContainerSpec,里面需要持有有效的TargerData和TargetGameplayEffectSpecs，不然此此调用将会无效
	*/
	UFUNCTION(BlueprintCallable, Category = Ability)
		static TArray<FActiveGameplayEffectHandle> ApplyExternalEffectContainerSpec(const FRPGGameplayEffectContainerSpec& Spec);

	/**
	* EffectContainerSpec 是否持有有效的Effect
	*/
	UFUNCTION(BlueprintPure, Category = Ability)
		static bool DoesEffectContainerSpecHaveEffects(const FRPGGameplayEffectContainerSpec& Spec);
};