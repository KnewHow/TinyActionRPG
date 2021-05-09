#include "Abilities/RPGDamageExecution.h"
#include "GameplayEffect.h"
#include "AbilitySystemComponent.h"
#include "RPGAttributeSet.h"
/**
* �ͼ����˺���صľ�̬�࣬���������Ƕ�׷�ٺͼ����˺��������
* ��Щ���Ա����� URPGAttributeSet ��
*/
struct RPGDamageStatics {
	DECLARE_ATTRIBUTE_CAPTUREDEF(DefensePower); // ������
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower); // ������
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage); // �����˺�

	RPGDamageStatics() {
		/**
		* ׷��Ŀ��ķ��������������ǲ�����Ŀ��ķ��������գ����ǵ�Ч����Ŀ����Ч��˲�䣬���ǻ�ȡĿ���
		* ��������ʵʱ���㡣
		*/
		DEFINE_ATTRIBUTE_CAPTUREDEF(URPGAttributeSet, DefensePower, Target, false);

		/**
		* ׷���˺���Դ�Ĺ��������գ����ڹ�����������ֻ��ȡ����ʱ��Ĺ�����
		* ���磺����һ�Ż��򣬷������ҹ��������ܻ���ΪĳЩԭ���������������ǻ�����
		* ��ҷ������ʱ��һʱ�̵Ĺ�������Ϊ�����׼���������׷�ٿ��յ�ֵ
		*/
		DEFINE_ATTRIBUTE_CAPTUREDEF(URPGAttributeSet, AttackPower, Source, true);

		/**
		* ׷�ٻ����˺�������Ҳ��׷���ͷż����ǿ̵Ŀ���ֵ
		*/
		DEFINE_ATTRIBUTE_CAPTUREDEF(URPGAttributeSet, Damage, Source, true);
	}
};

// ����һ����̬��������ȡ�˺�����
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

	// ��ʵ�˺��������˺�*������/������
	float DamgeDone = Damge * AttackPower / DefensePower;

	if (DamgeDone > 0.0f) {
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().DamageProperty, EGameplayModOp::Additive, DamgeDone));
	}
}
