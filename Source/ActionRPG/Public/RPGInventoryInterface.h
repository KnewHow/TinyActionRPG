#pragma once
#include "ActionRPG.h"
#include "RPGInventoryInterface.generated.h"

/**
* 一个和背包相关的接口，实现者需要完成如下的方法
*/

UINTERFACE(MinimalAPI, meta=(CannotImplementInterfaceInBlueprint))
class URPGInventoryInterface : public UInterface {
	GENERATED_BODY()
};

class ACTIONRPG_API IRPGInventoryInterface {
	GENERATED_BODY()
public:
	
	/**
	* 获取物品到物品数据的映射
	* @return 返回一个Map的映射
	*/
	virtual const TMap<URPGItem*, FRPGItemData>& GetInventoryDataMap() const = 0;

	/**
	* 获取插槽到商品的映射
	* @return 插槽到物品的映射
	*/
	virtual const TMap<FRPGItemSlot, URPGItem*>& GetSlottedItemMap() const = 0;

	/**
	* 获取背包物品改变的本地会话
	* @return 返回一个本地会话
	*/
	virtual FOnInventoryItemChangedNative& GetInventoryItemChangedDelegate() = 0;

	/**
	* 获取插槽改变本地会话
	* @param 返回一个本地会话
	*/
	virtual FOnSlotItemChangedNative& GetSlotItemChangedDelegate() = 0;

	/**
	* 获取背包加载完成的本地会话
	* @param 返回一个本地会话
	*/
	virtual FOnInventoryLoadedNative& GetInventoryLoadedDelegate() = 0;

};