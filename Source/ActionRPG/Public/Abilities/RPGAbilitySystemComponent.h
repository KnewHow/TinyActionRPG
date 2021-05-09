#pragma once

/**
* ����ϵͳ���
*/
#include "GameplayAbilities/Public/AbilitySystemComponent.h"
#include "RPGGameplayAbility.h"
#include "RPGAbilitySystemComponent.generated.h"

UCLASS()
class ACTIONRPG_API URPGAbilitySystemComponent : public UAbilitySystemComponent {
	GENERATED_BODY()
public:

	/**
	* ���ݱ�ǩ�鷵�ؿ��Լ��������
	* @param TagContainer ��ǩ��
	* @param ActiveAbilities ���صĿɼ�����������
	*/
	void GetActiveAbilitiesWithTags(const FGameplayTagContainer& TagContainer, TArray<URPGGameplayAbility*>& ActiveAbilities);

	/**
	* ��ȡĬ�ϵ�����
	*/
	int32 GetDefaultAbilityLevel() const;
	
	/**
	* ����Actorȥ����Actor��������AbilitySystemComponent
	* һ������Ҫ�󴫵�����Actor������Ҫʵ�� IAbilitySystemInterface �ӿ��������ѯ
	* @param Actor ���ݴ� Actor ������ AbilitySystemComponet ���
	* @param LookForComponent �Ƿ���� Actor������FindComponentByClass������ AbilitySystemCompent,һ��Ĭ��Ϊ false, ��˴˷�����Ч���ҽ�����ǲ������
	* @return ��� �ҵ��������ָ�룬���򷵻ؿ�ָ��
	*/
	static URPGAbilitySystemComponent* GetAbilitySystemComponentFromActor(const AActor* Actor, bool LookForComponent = false);
};