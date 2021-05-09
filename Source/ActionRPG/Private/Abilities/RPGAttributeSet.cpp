#include "RPGAttributeSet.h"
#include "GameplayAbilities/Public/GameplayEffectTypes.h"
#include "GameplayAbilities/Public/GameplayEffectExtension.h"
#include "RPGCharacterBase.h"

URPGAttributeSet::URPGAttributeSet() :
	Health(1.0f),
	MaxHealth(1.0f),
	Mana(0.0f),
	MaxMana(0.0f),
	AttackPower(1.0f),
	DefensePower(1.0f),
	Damage(0.0f),
	MoveSpeed(1.0f)
{

}


void URPGAttributeSet::AdjustAttributeForMaxChange(
	FGameplayAttributeData& AffectedAttribute, 
	const FGameplayAttributeData& MaxAttributeData, 
	float NewMaxValue, 
	const FGameplayAttribute& AffectedAttributeProperty)
{
	
	UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttributeData.GetCurrentValue();

	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue)) { // 如果当前最大值和需要修该的最大值不是近似相等
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		float NewDelta = (CurrentMaxValue > 0) ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue; // 获取需要修改属性的差值
		AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta); // 修改对应的属性值
	}
}

void URPGAttributeSet::OnRep_Health()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, Health);
}

void URPGAttributeSet::OnRep_MaxHealth()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, MaxHealth);
}

void URPGAttributeSet::OnRep_Mana()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, Mana);
}

void URPGAttributeSet::OnRep_MaxMana()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, MaxMana);
}

void URPGAttributeSet::OnRep_AttackPower()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, AttackPower);
}

void URPGAttributeSet::OnRep_DefensePower()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, DefensePower);
}

void URPGAttributeSet::OnRep_MoveSpeed()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, MoveSpeed);
}

void URPGAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(URPGAttributeSet, Health);
	DOREPLIFETIME(URPGAttributeSet, MaxHealth);
	DOREPLIFETIME(URPGAttributeSet, Mana);
	DOREPLIFETIME(URPGAttributeSet, MaxMana);
	DOREPLIFETIME(URPGAttributeSet, AttackPower);
	DOREPLIFETIME(URPGAttributeSet, DefensePower);
	DOREPLIFETIME(URPGAttributeSet, MoveSpeed);
}

void URPGAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	// 只针对MaxHealth 和 MaxMana的最大值修改调整对应的属性值
	if (Attribute == GetMaxHealthAttribute()) {
		AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
	}
	else if (Attribute == GetMaxManaAttribute()) {
		AdjustAttributeForMaxChange(Mana, MaxMana, NewValue, GetManaAttribute());
	}
}

void URPGAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent(); // 获取发起者的 AbilityComponet和Tags
	const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();

	float DeltaValue = 0;
	if (Data.EvaluatedData.ModifierOp == EGameplayModOp::Type::Additive) {
		DeltaValue = Data.EvaluatedData.Magnitude;
	}

	// 获取能力所作用的目标
	AActor* TargetActor = nullptr;
	AController* TargetController = nullptr;
	ARPGCharacterBase* TargetCharacter = nullptr;
	if (Data.Target.AbilityActorInfo.IsValid() &&
		Data.Target.AbilityActorInfo->AvatarActor.IsValid()) {
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		TargetCharacter = Cast<ARPGCharacterBase>(TargetActor);
	}

	if (Data.EvaluatedData.Attribute == GetDamageAttribute()) { // 伤害属性
		AActor* SourceActor = nullptr;
		AController* SourceController = nullptr;
		ARPGCharacterBase* SourceCharacter = nullptr;
		if (Source && Source->AbilityActorInfo.IsValid() && Source->AbilityActorInfo->AvatarActor.IsValid()) {
			SourceActor = Source->AbilityActorInfo->AvatarActor.Get();
			SourceController = Source->AbilityActorInfo->PlayerController.Get();
		
			if (SourceController == nullptr && SourceActor != nullptr) {
				if (APawn* Pawn = Cast<APawn>(SourceActor)) {
					SourceController = Pawn->GetController();
				}
			}

			if (SourceController) {
				SourceCharacter = Cast<ARPGCharacterBase>(SourceController->GetPawn());
			}
			else {
				SourceCharacter = Cast<ARPGCharacterBase>(SourceActor);
			}

			// 这里对SourceActor 做更精确的判定
			if (Context.GetEffectCauser()) {
				SourceActor = Context.GetEffectCauser();
			}
		}

		// 如果有碰撞信息，记录一下碰撞信息
		FHitResult HitResult;
		if (Context.GetHitResult()) {
			HitResult = *Context.GetHitResult();
		}
		// 获取并清除当前伤害值
		const float LocalDamage = GetDamage();
		SetDamage(0.0f);
		if (LocalDamage > 0.0f) {
			const float OldHealth = GetHealth();
			SetHealth(FMath::Clamp(OldHealth - LocalDamage, 0.0f, GetMaxHealth()));

			if (TargetCharacter) {
				TargetCharacter->HandleDamage(LocalDamage, HitResult, SourceTags, SourceCharacter, SourceActor);
				TargetCharacter->HandleHealthChange(-LocalDamage, SourceTags);
			}
		}
	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute()) {
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth())); // TODO 个人觉得这行代码有点多余
		if (TargetCharacter) {
			TargetCharacter->HandleManaChange(DeltaValue, SourceTags);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetManaAttribute()) {
		SetMana(FMath::Clamp(GetMana(), 0.0f, GetMaxMana()));
		if (TargetCharacter) {
			TargetCharacter->HandleManaChange(DeltaValue, SourceTags);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetMoveSpeedAttribute()) {
		if (TargetCharacter) {
			TargetCharacter->HandleMoveSpeed(DeltaValue, SourceTags);;
		}
	}
}
