#pragma once
/**
* 
* RPG PlayerController 基类, 这里我们主要负责玩家的控制和数据的变更
* 这里的数据变更主要是对游戏里面背包数据的加载、增加/移除、存储等操作，以及完成后通知给其他对象
* 
* 除此之外，这里的PlayController 还需要实现 `RPGInventoryInferface` 接口
* 在`RPGInventoryInferface`接口中，我们定义了很多和背包相关读操作和获取相关会话，
* 这样做的目的是为了让其他需要获取背包数据的类不强行依赖`PlayerController`，而只有引用`RPGInventoryInferface` 即可
* 
*/

#include "ActionRPG.h"
#include "RPGTypes.h"
#include "GameFramework/PlayerController.h"
#include "RPGInventoryInterface.h"
#include "RPGPlayerControllerBase.generated.h"


UCLASS()
class ACTIONRPG_API ARPGPlayerControllerBase : public APlayerController, public IRPGInventoryInterface {
	GENERATED_BODY()
public:

	ARPGPlayerControllerBase();

	virtual void BeginPlay() override;

	/**
	* 背包中物品和数据的映射
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly , Category = Inventory)
		TMap<URPGItem*, FRPGItemData> InventoryData;

	/**
	* 插槽到物品的映射
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
		TMap<FRPGItemSlot, URPGItem*> SlottedItems;

	/**
	* 背包物品变更蓝图会话和本地会话
	*/
	UPROPERTY(BlueprintAssignable, Category = Inventory)
		FOnInventoryItemChanged OnInventoryItemChangedDelegate;
	FOnInventoryItemChangedNative OnInventoryItemChangedNativeDelegate;

	/**
	* 背包插槽变更的蓝图会话与本地会话
	*/
	UPROPERTY(BlueprintAssignable, Category = Inventory)
		FOnSlotItemChanged OnSlotItemChangedDelegate;
	FOnSlotItemChangedNative OnSlotItemChangedNativeDelegate;

	/**
	* 背包加载完成蓝图会话和本地会话
	*/
	UPROPERTY(BlueprintAssignable, Category = Inventory)
		FOnInventoryLoaded OnInventoryLoadedDelegate;
	FOnInventoryLoadedNative OnInventoryLoadedNativeDelegate;
	
	// ---------Implement from IRPGInventoryInterface start---------
	virtual const TMap<URPGItem*, FRPGItemData>& GetInventoryDataMap() const override;
	virtual const TMap<FRPGItemSlot, URPGItem*>& GetSlottedItemMap() const override;
	virtual FOnInventoryItemChangedNative& GetInventoryItemChangedDelegate() override;
	virtual FOnSlotItemChangedNative& GetSlotItemChangedDelegate() override;
	virtual FOnInventoryLoadedNative& GetInventoryLoadedDelegate() override;
	// ---------Implement from IRPGInventoryInterface end---------


	/**
	* 背包物品变更事件，此函数需要蓝图单独去实现
	* @param bAdded 该物品是否已经被添加到背包中
	* @param Item 新的物品
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = Inventory)
		void InventoryItemChanged(bool bAdded, URPGItem* Item);

	/**
	* 插槽变更事件，此函数需要蓝图单独实现
	* @param ItemSlot 变更的插槽
	* @param Item 插槽对应的物品
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = Inventory)
		void SlottedItemChanged(FRPGItemSlot ItemSlot, URPGItem* Item);

	/**
	* 添加物品到背包中
	* @param NewItem 新的物品
	* @param ItemCount 物品的数量，默认为：1
	* @param ItemLevel 物品等级，默认为：1
	* @param bAutoSlot 是否被添加到插槽中，默认为：true
	* @return bool 如果添加成功返回 true，否则返回 false
	*/
	UFUNCTION(BlueprintCallable, Category = Inventory)
		bool AddInventoryItem(URPGItem* NewItem, int32 ItemCount = 1, int32 ItemLevel = 1, bool bAutoSlot = true);

	/**
	* 从背包中移除指定数量的物品，如果该物品被全部移除完毕，该物品也会被从插槽中取消掉
	* @param RemovedItem 需要被移除的物品
	* @param RemovedCount 需要被移除的数量，默认值为：1
	* @return bool 移除成功返回 true,否则返回 false
	*/
	UFUNCTION(BlueprintCallable, Category = Inventory)
		bool RemoveInventoryItem(URPGItem* RemovedItem, int32 RemoveCount = 1);

	/**
	* 根据类型来获取背包中物品，如果类型为空，则返回所有背包中的物品
	* @param Items 需要返回的物品信息
	* @param ItemType 物品类型，可以为空
	*/
	UFUNCTION(BlueprintCallable, Category = Inventory)
		void GetInventoryItems(TArray<URPGItem*>& Items, FPrimaryAssetType ItemType) const;
	
	/**
	* 根据物品信息来获取物品在背包中数量，如果该物品不存在，则返回0
	* @param Item 物品信息
	* @return 物品在背包中的数量
	*/
	UFUNCTION(BlueprintCallable, Category = Inventory)
		int32 GetInventoryItemCount(URPGItem* Item) const;

	/**
	* 根据物品信息获取物品数据信息
	* @param Item 物品信息
	* @param ItemData 需要返回的物品数据
	* @return 如果物品合法，返回true,否则返回false
	*/
	UFUNCTION(BlueprintCallable, Category = Inventory)
		bool GetInventoryItemData(URPGItem* Item, FRPGItemData& ItemData) const;

	/**
	* 设置插槽中的物品，如果找到该插槽，设置他的物品为Item,如果没找到，但是找到Item在另一个物品中，则移除该插槽的物品，上述两种方式都会发起ItemSlotChanged调用
	* @param ItemSlot 物品插槽
	* @param Item 需要放到插槽里面的物品
	* @return 如果设置成功，返回true,否则返回false
	*/
	UFUNCTION(BlueprintCallable, Category = Inventory)
		bool SetSlottedItem(FRPGItemSlot ItemSlot, URPGItem* Item);
	
	/**
	* 根据插槽获取物品信息
	* @param ItemSlot 物品插槽
	* @return 物品信息，如果找到Item,返回物品，否则返回nullptr
	*/
	UFUNCTION(BlueprintCallable, Category = Inventory)
		URPGItem* GetSlottedItem(FRPGItemSlot ItemSlot)const;

	/**
	* 根据类型获取插槽中的物品，而不是背包中的物品，区别于[GetInventoryItems]
	* @param Items 最后返回的物品
	* @param ItemType 物品类型，如果为空，则返回全部
	*/
	UFUNCTION(BlueprintCallable, Category = Inventory)
		void GetSlottedItems(TArray<URPGItem*>& Items, FPrimaryAssetType ItemType) const;
	
	/**
	* 用背包里面的物品填充插槽，如果插槽已经被同类型的物品填充，则不覆盖，如果插槽为空，则使用新的物品覆盖
	* 如果插槽有变动，则将这些变动存档
	*/
	UFUNCTION(BlueprintCallable, Category = Inventory)
		void FillEmptySlots();

	/**
	* 将当前背包中的数据同步到SaveGame中，并且存储到磁盘中。
	* 一般在背包发生变更时调用，而且对磁盘的写入操作是异步的，不会影响游戏性能
	* @return 如果存储成功，则返回true，否则返回false
	*/
	UFUNCTION(BlueprintCallable, Category = Inventory)
		bool SaveInventory();

	/**
	* 将SaveGame中的存档数据同步到当前背包中，一般在游戏开始时调用
	* 此方法会内部调用[NotifyInventoryLoaded]方法来通知背包已加载
	* @return 如果加载成功，返回true,否则返回false
	*/
	UFUNCTION(BlueprintCallable, Category = Inventory)
		bool LoadInventory();

protected:
	/**
	* 使用新的物品来填充空的插槽
	* @param NewItem 新的物品
	* @return bool 填充成功返回 true,填充失败返回 false
	*/
	bool FillEmptySlotWithItem(URPGItem* NewItem);

	// --------------------下面是一些变更的通知函数------------------
	
	/**
	* 当背包物品改变时，调用此函数发起通知
	* @param bAdded 物品是否已被添加入背包
	* @param Item 需要添加的物品
	*/
	void NotifyInventoryItemChanged(bool bAdded, URPGItem* Item);

	/**
	* 当插槽变更时，调用此函数执行通知
	* @param ItemSlot 变更的插槽
	* @param Item 插槽对应的物品
	*/
	void NotifySlottedItemChanged(FRPGItemSlot ItemSlot, URPGItem* Item);

	/**
	* 当背包数据被加载完成时，调用此函数通知
	*/
	void NotifyInventoryLoaded();

	/**
	* 此函数被绑定到GameInstance中，由GameInstance决定何时去加载存档
	* 
	*/
	void HandleSaveGameLoad(URPGSaveGame* SaveGame);
};
