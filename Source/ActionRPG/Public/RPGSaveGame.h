#pragma once

/**
* 自定义的SaveGame,用于存档数据
*/
#include "ActionRPG.h"
#include "RPGTypes.h"
#include "GameFramework/SaveGame.h"
#include "RPGSaveGame.generated.h"

/**
* 
* 定义一写关于SaveGame的版本号，
* 本地的c++代码将会对不同的版本做不同的处理，并且会修正一些版本号
* 个人觉得这个在本项目中好像是多余的，完全没有用！！
*/
namespace ERPGSaveGameVersion {
	enum type
	{
		Initial, // 初始版本
		AddedInventory, // 数据已经被添加至背包中
		AddedItemData, // 添加了 ItemData 
		//-------------------如果有新的版本，必须要在此横线之前添加，不能添加在此横线之后-------------------------------
		VersionPlusOne,
		LatestVersion = VersionPlusOne - 1 // 最新版本
	};
}

UCLASS(BlueprintType)
class ACTIONRPG_API URPGSaveGame : public USaveGame {
	
	GENERATED_BODY()

public:
	URPGSaveGame();

	/**
	* 物品和物品数据之间的映射，可以反应每个物品的等级和数量
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveGame)
		TMap<FPrimaryAssetId, FRPGItemData> InventoryData;

	/**
	* 插槽和商品的映射
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveGame)
		TMap<FRPGItemSlot, FPrimaryAssetId> SlotItems;

	/**
	* 用户的唯一ID
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveGame)
		FString UserId;

protected:
	/**
	* 一些默认的物品，这里只负责读取，
	* 然而我觉得这个东西完全没用
	* TODO: 检查有用性
	*/
	UPROPERTY()
		TArray<FPrimaryAssetId> InventoryItems_DEPRECATED;
	
	/**
	* 版本号，是最上面声明的版本号中的一个
	*/
	UPROPERTY()
		int32 SaveDataVersion;

	/**
	* 序列化方法，用来修复版本和默认读取，个人觉得没有什么用，因为用于执行不到核心的代码
	* TODO: 检测有用性
	* 
	*/
	virtual void Serialize(FArchive& Ar) override;
};