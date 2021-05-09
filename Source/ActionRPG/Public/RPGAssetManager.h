#pragma once

/**
* 自定义资源管理器，用于加载自定义的物品资源，
* 并且定义了各种物品资源类型
*/

#include "Engine/AssetManager.h"
#include "ActionRPG.h"
#include "RPGAssetManager.generated.h"

class URPGItem;

UCLASS()
class ACTIONRPG_API URPGAssetManager : public UAssetManager {
	GENERATED_BODY()
public:
	/**
	* 默认构造函数
	*/
	URPGAssetManager() {};
	/**
	* 开始初始化加载
	*/
	virtual void StartInitialLoading() override;

	/**
	* 在这里定义所有商品的类型
	*/
	static const FPrimaryAssetType PotionItemType; // 药品类型
	static const FPrimaryAssetType SkillItemType; // 技能类型
	static const FPrimaryAssetType TokenItemType; // 金币类型
	static const FPrimaryAssetType WeaponItemType; // 武器类型

	/**
	* 获取当前的资源管理器
	*/
	static URPGAssetManager& Get();
	
	/**
	* 根据资源ID同步加载资源，此方法可能会阻塞，但是对于不能等待的资源，是非常有用的
	* @param PrimaryAssetId 资源的唯一标识符
	* @param bLogWarning 是否需要记录日志
	*/
	URPGItem* ForceLoadItem(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning = false);
};