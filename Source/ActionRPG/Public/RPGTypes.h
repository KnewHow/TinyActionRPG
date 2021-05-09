#pragma once

//-----------------------------------------------
// 该头文件放置全局的枚举和一些常用的数据类，这些东西可以被代码和蓝图使用
// 将这些全局通用的类型集中到一个单独的头文件中可以减少循环引用的问题
// 如果可以，你要可以把`数据表`的结构类型放到这里
//-----------------------------------------------


#include "UObject/PrimaryAssetId.h"
#include "RPGTypes.generated.h"

class URPGItem;
class URPGSaveGame;

/**
* 商品插槽信息，用于被UI显示
*/
USTRUCT(BlueprintType)
struct ACTIONRPG_API FRPGItemSlot {
	
	GENERATED_BODY()

	/**
	* 默认构造函数，插槽数默认为-1
	*/
	FRPGItemSlot() 
		: SlotNumber(-1)
	{}

	/**
	* 构造函数
	* @param InItemType 商品类型
	* @param InSlotNumber 插槽数量
	*/
	FRPGItemSlot(const FPrimaryAssetType& InItemType, int32 InSlotNumber)
		: ItemType(InItemType), SlotNumber(InSlotNumber)
	{}


	/**
	* 插槽中商品的类型
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	FPrimaryAssetType ItemType;

	/**
	* 插槽数量
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	int32 SlotNumber;

	bool operator==(const FRPGItemSlot& other) const {
		return ItemType == other.ItemType && SlotNumber == other.SlotNumber;
	}

	bool operator!=(const FRPGItemSlot& other) const {
		return !(*this == other);
	}

	/**
	* 为了让 FRPGItemSlot 在Map/Set 计算Hash值的时候更方便，我们实现此方法
	*/
	friend inline uint32 GetTypeHash(const FRPGItemSlot& Key) {
		uint32 Hash = 0;
		Hash = HashCombine(Hash, GetTypeHash(Key.ItemType));;
		Hash = HashCombine(Hash, (uint32)Key.SlotNumber);
		return Hash;
	}

	/**
	* 该插槽是否合法
	*/
	bool IsValid() const {
		return ItemType.IsValid() && SlotNumber >= 0;
	}
};

/**
* 背包中的商品信息，包含商品的数量和等级
*/
USTRUCT(BlueprintType)
struct ACTIONRPG_API FRPGItemData {
	GENERATED_BODY()
	
	FRPGItemData()
	:Count(1), Level(1)
	{}

	FRPGItemData(int32 Counter, int32 Level)
		:Count(Counter), Level(Level)
	{}
	/**
	* 商品在背包中的数量，不能小于1
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
		int32 Count;

	/**
	* 商品等级，此等级将会被所有实例所共享
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
		int32 Level;


	bool operator==(const FRPGItemData& Other) const {
		return Count == Other.Count && Level == Other.Count;
	}

	bool operator!=(const FRPGItemData& Other) const {
		return !(*this == Other);
	}

	bool IsValid() const {
		return Count > 0;
	}

	/**
	* 更新数据
	* @param Other 需要更新的数据
	* @param MaxCount 最大数量限制
	* @param MaxLevel 最大等级限制
	*/
	void UpdateItemData(const FRPGItemData& Other, int32 MaxCount, int32 MaxLevel) {
		// 如果最大限制小于等于0，就置为INT32_MAX，不限制
		if (MaxCount <= 0) {
			MaxCount = INT32_MAX;
		}
		if (MaxLevel <= 0) {
			MaxLevel = INT32_MAX;
		}

		Count = FMath::Clamp(Count + Other.Count, 1, MaxCount);
		Level = FMath::Clamp(Level + Other.Level, 1, MaxCount);
	}
};


//------------------------------------------------
// 以下是一些会话的声明
//------------------------------------------------

// 背包物品改变会话
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryItemChanged, bool, bAdded, URPGItem*, Item);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInventoryItemChangedNative, bool, URPGItem*);

// 背包插槽改变会话
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSlotItemChanged, FRPGItemSlot, ItemSlot, URPGItem*, Item);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSlotItemChangedNative, FRPGItemSlot, URPGItem*);

// 背包加载完成会话
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryLoaded);
DECLARE_MULTICAST_DELEGATE(FOnInventoryLoadedNative);

// SaveGame 加载完成会话
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveGameLoaded, URPGSaveGame*, SaveGame);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSaveGameLoadedNative, URPGSaveGame*);