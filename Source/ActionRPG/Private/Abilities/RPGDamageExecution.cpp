#include "Abilities/RPGDamageExecution.h"
#include "GameplayEffect.h"
#include "AbilitySystemComponent.h"
#include "RPGAttributeSet.h"
/**
* 和计算伤害相关的静态类，在其中我们对追踪和计算伤害相关属性
* 这些属性保存在 URPGAttributeSet 中
*/
struct RPGDamageStatics {
	DECLARE_ATTRIBUTE_CAPTUREDEF(DefensePower); // 防御力
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower); // 攻击力
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage); // 基础伤害

	RPGDamageStatics() {
		/**
		* 追踪目标的防御力，这里我们不是拿目标的防御力快照，而是当效果对目标生效的瞬间，我们获取目标的
		* 防御力来实时计算。
		*/
		DEFINE_ATTRIBUTE_CAPTUREDEF(URPGAttributeSet, DefensePower, Target, false);

		/**
		* 追踪伤害来源的攻击力快照，对于攻击力，我们只获取攻击时候的攻击力
		* 例如：发射一颗火球，发射后玩家攻击力可能会因为某些原因提升，但是我们还是以
		* 玩家发射火球时那一时刻的攻击力作为计算标准。因此这里追踪快照的值
		*/
		DEFINE_ATTRIBUTE_CAPTUREDEF(URPGAttributeSet, AttackPower, Source, true);

		/**
		* 追踪基础伤害，这里也是追踪释放技能那刻的快照值
		*/
		DEFINE_ATTRIBUTE_CAPTUREDEF(URPGAttributeSet, Damage, Source, true);
	}
};

// 定义一个静态方法来获取伤害因子
static const RPGDamageStatics& DamageStatics() {
	static RPGDamageStatics DmgStatics;
	return DmgStatics;
}

URPGDamageExecution::URPGDamageExecution() {
	RelevantAttributesToCapture.Add(DamageStatics().DefensePowerDef);
	RelevantAttributesToCapture.Add(DamageStatics().AttackPowerDef);
	RelevantAttributesToCapture.Add(DamageStatics().DamageDef);
}

void URPGDamageExecution::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams, 
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();

	AActor* TargetActor = TargetAbilitySystemComponent ? TargetAbilitySystemComponent->AvatarActor : nullptr;
	AActor* SourceActor = SourceAbilitySystemComponent ? SourceAbilitySystemComponent->AvatarActor : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.TargetTags = TargetTags;
	EvaluateParameters.SourceTags = SourceTags;

	float DefensePower = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DefensePowerDef, EvaluateParameters, DefensePower);
	if (DefensePower == 0.0f) {
		DefensePower = 1.0f;
	}

	float AttackPower = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().AttackPowerDef, EvaluateParameters, AttackPower);

	float Damge = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageDef, EvaluateParameters, Damge);

	// 真实伤害：基础伤害*攻击力/防御力
	float DamgeDone = Damge * AttackPower / DefensePower;

	if (DamgeDone > 0.0f) {
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().DamageProperty, EGameplayModOp::Additive, DamgeDone));
	}
}
