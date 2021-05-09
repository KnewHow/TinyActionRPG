#pragma once

/**
* 和能力系统相关的基础属性，例如：血量、法力值、攻击力等等
* 如果有必要的话，每个角色都需要引用该类型
*/
#include "GameplayAbilities/Public/AttributeSet.h"
#include "RPGAbilitySystemComponent.h"
#include "RPGAttributeSet.generated.h"

// 使用宏来定义Attribute属性的访问器
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
	

UCLASS()
class ACTIONRPG_API URPGAttributeSet : public UAttributeSet {
	GENERATED_BODY()
public:

	/**
	* 默认空构造函数，用来初始化默认属性值
	*/
	URPGAttributeSet();

	/**
	* 修改前的预处理方法，在此方法中，
	* 我们将对修改的值进行校验，并且做最大值上限改变的调整策略
	* @param Attribute 需要修改的属性
	* @param NewValue 需要修改的值
	*/
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue);


	/**
	* 提交伤害，并且做一些显示和通知的工作
	*/
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data);

	
	/**
	* 获取被复制属性的声明周期，一般可复制属性，都需要实现此方法
	* @param OutLifetimeProps 返回可复制属性的声明周期
	*/
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const;


	/**
	* 当前血量
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health)
		FGameplayAttributeData Health;
		ATTRIBUTE_ACCESSORS(URPGAttributeSet, Health);

	/**
	* 最大血量上限
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_MaxHealth)
		FGameplayAttributeData MaxHealth;
		ATTRIBUTE_ACCESSORS(URPGAttributeSet, MaxHealth);


	/**
	* 当前法力值
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Mana", ReplicatedUsing = OnRep_Mana)
		FGameplayAttributeData Mana;
		ATTRIBUTE_ACCESSORS(URPGAttributeSet, Mana);

	/**
	* 法力值上限
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Mana", ReplicatedUsing = OnRep_MaxMana)
		FGameplayAttributeData MaxMana;
		ATTRIBUTE_ACCESSORS(URPGAttributeSet, MaxMana);

	/**
	* 攻击力，用来计算伤害的一个因子
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Damage", ReplicatedUsing = OnRep_AttackPower)
		FGameplayAttributeData AttackPower;
		ATTRIBUTE_ACCESSORS(URPGAttributeSet, AttackPower);

	/**
	* 防御力，用来计算伤害的一个因子
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Damage", ReplicatedUsing = OnRep_DefensePower)
		FGameplayAttributeData DefensePower;
		ATTRIBUTE_ACCESSORS(URPGAttributeSet, DefensePower);
    

	/**
	* 伤害值，这是一个临时的属性，由 DamageExecution 来计算最终的伤害
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Damage")
		FGameplayAttributeData Damage;
		ATTRIBUTE_ACCESSORS(URPGAttributeSet, Damage);


	/**
	* 移动速度
	*/
	UPROPERTY(BlueprintReadOnly, Category = "MoveSpeed", ReplicatedUsing = OnRep_MoveSpeed)
		FGameplayAttributeData MoveSpeed;
		ATTRIBUTE_ACCESSORS(URPGAttributeSet, MoveSpeed);

protected:

	/**
	* 一个帮助函数，当一个属性的最大值改变时，对应调整他对应的属性
	* @param AffectedAttribute 该属性原值
	* @param MaxAttributeData 该属性的上限(最大)值
	* @param NewMaxValue 新的最大值
	* @param AffectedAttributeProperty 该属性对象，此参数需要传递给 AbilitySystem 进行修改
	*/
	void  AdjustAttributeForMaxChange(
		FGameplayAttributeData& AffectedAttribute,
		const FGameplayAttributeData& MaxAttributeData,
		float NewMaxValue,
		const FGameplayAttribute& AffectedAttributeProperty
	);
	/*
	* --------------------------------------------
    * 属性网络复制时调用的函数
    * -------------------------------------------- 
	*/
	UFUNCTION()
		virtual void OnRep_Health();

	UFUNCTION()
		virtual void OnRep_MaxHealth();

	UFUNCTION()
		virtual void OnRep_Mana();

	UFUNCTION()
		virtual void OnRep_MaxMana();

	UFUNCTION()
		virtual void OnRep_AttackPower();

	UFUNCTION()
		virtual void OnRep_DefensePower();


	UFUNCTION()
		virtual void OnRep_MoveSpeed();
};