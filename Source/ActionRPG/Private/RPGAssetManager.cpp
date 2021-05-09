
#include "RPGAssetManager.h"
#include "Item/RPGItem.h"
#include "AbilitySystemGlobals.h"

const FPrimaryAssetType URPGAssetManager::PotionItemType = TEXT("Potion"); 
const FPrimaryAssetType URPGAssetManager::SkillItemType = TEXT("Skill");
const FPrimaryAssetType URPGAssetManager::TokenItemType = TEXT("Token");
const FPrimaryAssetType URPGAssetManager::WeaponItemType = TEXT("Weapon");


void URPGAssetManager::StartInitialLoading()
{
	// 调用父类，扫描初始化资源
	Super::StartInitialLoading();
	// 加载全局的数据表和标签
	UAbilitySystemGlobals::Get().InitGlobalData();

}

URPGAssetManager& URPGAssetManager::Get()
{	// 从引擎配置中获取资源管理器
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
