#pragma once
#include "ActionRPG.h"
#include "RPGTypes.h"
#include "Engine/GameInstance.h"
#include "RPGGameInstanceBase.generated.h"

/**
* RPGGameInstance 基类，定义了一系列默认的参数，
* 例如初始背包里面的资源类型和数量，以及每种资源的插槽数量和一些和加载SaveGame的属性和方法
*/

UCLASS()
class ACTIONRPG_API URPGGameInstanceBase : public UGameInstance {
	GENERATED_BODY()
public:
	URPGGameInstanceBase();

	/**
	* 默认的背包数据，定义每种资源对应的数量
	*/
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = Inventory)
		TMap<FPrimaryAssetId, FRPGItemData> DefaultInventory;

	/**
	* 每种资源所拥有插槽的数量，在GameInstance中预先定义，便于后续初始化背包数据
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
		TMap<FPrimaryAssetType, int32> ItemSlotPerType;

	/**
	* 保存 SaveGame 的插槽名称
	*/
	UPROPERTY(BlueprintReadWrite, Category = Save)
		FString SaveSlot;

	/**
	* 保存SaveGame的 user index
	*/
	UPROPERTY(BlueprintReadWrite, Category = Save)
		int32 SaveUserIndex;

	/**
	* SaveGame 已加载会话，用于蓝图调用
	*/
	UPROPERTY(BlueprintAssignable, Category = Inventory)
		FOnSaveGameLoaded OnSaveGameLoadedDelegate;

	/**
	* SaveGame 已加载的本地会话
	*/
	FOnSaveGameLoadedNative OnSaveGameLoadedNativeDelegate;

	/**
	* 向SaveGame中添加默认的背包数据，这里需要保证SaveGame不能为nullpter
	* @param SaveGame 背包数据
	* @param bRemoveExtra 是否先清空背包中的默认数据
	*/
	UFUNCTION(BlueprintCallable, Category = Inventory)
		void AddDefaultInventory(URPGSaveGame* SaveGame, bool bRemoveExtra = false);

	/**
	* 判断是否是合法的插槽
	* @param ItemSlot 插槽信息
	* @return true 为有效，否则为无效
	*/
	UFUNCTION(BlueprintCallable, Category = Inventory)
		bool isValidItemSlot(FRPGItemSlot ItemSlot) const;
	
	/**
	* 获取当前的SaveGame
	* @return 返回当前SaveGame的指针
	*/
	UFUNCTION(BlueprintCallable, Category = Save)
		URPGSaveGame* GetCurrentSaveGame();

	/**
	* 设置当前SaveGame的状态为是否 可保存和可加载
	*/
	UFUNCTION(BlueprintCallable, Category = Save)
		void SetSavingEnabled(bool bEnabled);

	/**
	* 加载或者新建一个SaveGame,并且对SaveGame做一些处理，此方法对调用[HandleSaveGameLoaded] 来初始化加载或者创建的SaveGame
	* 如果磁盘中存在SaveGame,就加载，否则就创建。此方法调用的加载SaveGame的方法是同步，因此可能会造成线程阻塞
	* @return 如果是从磁盘加载的SaveGame返回true,如果是创建的返回 false
	*/
	UFUNCTION(BlueprintCallable, Category = Save)
		bool LoadOrCreateSaveGame();

	/**
	* 处理从磁盘中加载的SaveGame，此方法会使用默认的背包数据来选择初始化SaveGame
	* 如果SaveGame中已经包含了默认的背包数据项，则不做处理，否则用默认的背包项来初始化SaveGame
	* 由于参数中的 SaveGame 可能为空，因此当 SaveGame为空时，本函数将新建一个SaveGame对象，然后用背包默认数据来初始化它
	* 此方法会广播[OnSaveGameLoaded]和][OnSaveGameLoadedNative]会话
	* @return bool 如果SaveGame不为nullpter，返回true,否则返回false
	*/
	UFUNCTION(BlueprintCallable, Category = Save)
		bool HandleSaveGameLoaded(USaveGame* SaveGame);

	/**
	* 获取存储的插槽信息，包含插槽名称(SlotName)和用户索引(UserIndex)
	* @param SlotName 返回的插槽名称
	* @param UserIndex 返回的用户索引
	*/
	UFUNCTION(BlueprintCallable, Category = Save)
		void GetSaveSlotInfo(FString& SlotName, int32& UserIndex) const;

	/**
	* 将SaveGame写入磁盘，此方法将通过异步的方式将SaveGame写入磁盘
	* @return 如果写入成功返回true，否则返回false
	*/
	UFUNCTION(BlueprintCallable, Category = Save)
		bool WriteSaveGame();

	/**
	* 重置SaveGame，即使用默认的背包数据来初始化SaveGame中的数据
	* 此方法会广播[OnSaveGameLoaded]和][OnSaveGameLoadedNative]会话
	*/
	UFUNCTION(BlueprintCallable, Category = Save)
		void ResetSaveGame();

protected:
	/**
	* 当前的SaveGame
	*/
	UPROPERTY()
		URPGSaveGame* CurrentSaveGame;

	/**
	* 是否SaveGame可以被保存到磁盘中
	*/
	UPROPERTY()
		bool bSavingEnabled;
	
	/**
	* 当前是否正在保存中，用于保存排队
	*/
	UPROPERTY()
		bool bCurrentlySaving;

	/**
	* 当前是否有提交保存的请求，此参数用于当有两次连续保存请求时
	* 我们会先处理其中一个，将bPendingSaveRequested置为true,在完成上一次保存后，再次执行保存（此操作在回调函数HandleAsyncSave执行）
	*/
	UPROPERTY()
		bool bPendingSaveRequested;

	/**
	* 异步写入磁盘成功后的回调函数
	*/
	virtual void HandleAsyncSave(const FString& SlotName, const int32 UserIndex, bool bSuccess);
};