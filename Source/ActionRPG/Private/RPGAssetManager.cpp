
#include "RPGAssetManager.h"
#include "Item/RPGItem.h"
#include "AbilitySystemGlobals.h"

const FPrimaryAssetType URPGAssetManager::PotionItemType = TEXT("Potion"); 
const FPrimaryAssetType URPGAssetManager::SkillItemType = TEXT("Skill");
const FPrimaryAssetType URPGAssetManager::TokenItemType = TEXT("Token");
const FPrimaryAssetType URPGAssetManager::WeaponItemType = TEXT("Weapon");


void URPGAssetManager::StartInitialLoading()
{
	// ���ø��࣬ɨ���ʼ����Դ
	Super::StartInitialLoading();
	// ����ȫ�ֵ����ݱ�ͱ�ǩ
	UAbilitySystemGlobals::Get().InitGlobalData();

}

URPGAssetManager& URPGAssetManager::Get()
{	// �����������л�ȡ��Դ������
	URPGAssetManager* This = Cast<URPGAssetManager>(GEngine->AssetManager);
	if (This) {
		return *This;
	}
	else {
		UE_LOG(LogActionRPG, Fatal, TEXT("Invalid AssetManager in DefaultEngine.ini, must be RPGAssetManager"));
		return *NewObject<URPGAssetManager>(); // never call this, you should config RPGAssetManager in DefaultEngin.ini
	}
}

URPGItem* URPGAssetManager::ForceLoadItem(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning)
{
	FSoftObjectPath SoftObjectPath = GetPrimaryAssetPath(PrimaryAssetId);

	URPGItem* Item = Cast<URPGItem>(SoftObjectPath.TryLoad());
	if (Item == nullptr && bLogWarning) {
		UE_LOG(LogActionRPG, Warning, TEXT("Falied to load item from identifier: %s"), *PrimaryAssetId.ToString());
	}
	return Item;
}
