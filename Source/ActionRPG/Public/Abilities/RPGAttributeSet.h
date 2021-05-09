#pragma once

/**
* ������ϵͳ��صĻ������ԣ����磺Ѫ��������ֵ���������ȵ�
* ����б�Ҫ�Ļ���ÿ����ɫ����Ҫ���ø�����
*/
#include "GameplayAbilities/Public/AttributeSet.h"
#include "RPGAbilitySystemComponent.h"
#include "RPGAttributeSet.generated.h"

// ʹ�ú�������Attribute���Եķ�����
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
	* Ĭ�Ͽչ��캯����������ʼ��Ĭ������ֵ
	*/
	URPGAttributeSet();

	/**
	* �޸�ǰ��Ԥ���������ڴ˷����У�
	* ���ǽ����޸ĵ�ֵ����У�飬���������ֵ���޸ı�ĵ�������
	* @param Attribute ��Ҫ�޸ĵ�����
	* @param NewValue ��Ҫ�޸ĵ�ֵ
	*/
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue);


	/**
	* �ύ�˺���������һЩ��ʾ��֪ͨ�Ĺ���
	*/
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data);

	
	/**
	* ��ȡ���������Ե��������ڣ�һ��ɸ������ԣ�����Ҫʵ�ִ˷���
	* @param OutLifetimeProps ���ؿɸ������Ե���������
	*/
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const;


	/**
	* ��ǰѪ��
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health)
		FGameplayAttributeData Health;
		ATTRIBUTE_ACCESSORS(URPGAttributeSet, Health);

	/**
	* ���Ѫ������
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_MaxHealth)
		FGameplayAttributeData MaxHealth;
		ATTRIBUTE_ACCESSORS(URPGAttributeSet, MaxHealth);


	/**
	* ��ǰ����ֵ
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Mana", ReplicatedUsing = OnRep_Mana)
		FGameplayAttributeData Mana;
		ATTRIBUTE_ACCESSORS(URPGAttributeSet, Mana);

	/**
	* ����ֵ����
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Mana", ReplicatedUsing = OnRep_MaxMana)
		FGameplayAttributeData MaxMana;
		ATTRIBUTE_ACCESSORS(URPGAttributeSet, MaxMana);

	/**
	* �����������������˺���һ������
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Damage", ReplicatedUsing = OnRep_AttackPower)
		FGameplayAttributeData AttackPower;
		ATTRIBUTE_ACCESSORS(URPGAttributeSet, AttackPower);

	/**
	* �����������������˺���һ������
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Damage", ReplicatedUsing = OnRep_DefensePower)
		FGameplayAttributeData DefensePower;
		ATTRIBUTE_ACCESSORS(URPGAttributeSet, DefensePower);
    

	/**
	* �˺�ֵ������һ����ʱ�����ԣ��� DamageExecution ���������յ��˺�
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Damage")
		FGameplayAttributeData Damage;
		ATTRIBUTE_ACCESSORS(URPGAttributeSet, Damage);


	/**
	* �ƶ��ٶ�
	*/
	UPROPERTY(BlueprintReadOnly, Category = "MoveSpeed", ReplicatedUsing = OnRep_MoveSpeed)
		FGameplayAttributeData MoveSpeed;
		ATTRIBUTE_ACCESSORS(URPGAttributeSet, MoveSpeed);

protected:

	/**
	* һ��������������һ�����Ե����ֵ�ı�ʱ����Ӧ��������Ӧ������
	* @param AffectedAttribute ������ԭֵ
	* @param MaxAttributeData �����Ե�����(���)ֵ
	* @param NewMaxValue �µ����ֵ
	* @param AffectedAttributeProperty �����Զ��󣬴˲�����Ҫ���ݸ� AbilitySystem �����޸�
	*/
	void  AdjustAttributeForMaxChange(
		FGameplayAttributeData& AffectedAttribute,
		const FGameplayAttributeData& MaxAttributeData,
		float NewMaxValue,
		const FGameplayAttribute& AffectedAttributeProperty
	);
	/*
	* --------------------------------------------
    * �������縴��ʱ���õĺ���
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