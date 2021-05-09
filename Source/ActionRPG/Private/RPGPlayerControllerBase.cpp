#include "RPGPlayerControllerBase.h"
#include "Item/RPGItem.h"
#include "RPGGameInstanceBase.h"
#include "RPGAssetManager.h"
#include "RPGSaveGame.h"

ARPGPlayerControllerBase::ARPGPlayerControllerBase() {}

void ARPGPlayerControllerBase::BeginPlay()
{
	LoadInventory(); // 在游戏开始之前，先加载背包信息
	Super::BeginPlay();
}

const TMap<URPGItem*, FRPGItemData>& ARPGPlayerControllerBase::GetInventoryDataMap() const {
	return this->InventoryData;
}

const TMap<FRPGItemSlot, URPGItem*>& ARPGPlayerControllerBase::GetSlottedItemMap() const {
	return this->SlottedItems;
}

FOnInventoryItemChangedNative& ARPGPlayerControllerBase::GetInventoryItemChangedDelegate() {
	return this->OnInventoryItemChangedNativeDelegate;
}

FOnSlotItemChangedNative& ARPGPlayerControllerBase::GetSlotItemChangedDelegate()
{
	return this->OnSlotItemChangedNativeDelegate;
}

FOnInventoryLoadedNative& ARPGPlayerControllerBase::GetInventoryLoadedDelegate()
{
	return this->OnInventoryLoadedNativeDelegate;
}

bool ARPGPlayerControllerBase::AddInventoryItem(URPGItem* NewItem, int32 ItemCount, int32 ItemLevel, bool bAutoSlot)
{
	if (!NewItem) {
		UE_LOG(LogActionRPG, Error, TEXT("ARPGPlayerControllerBase.AddInventoryItem Error: NewItem is nullpter"));
		return false;
	}
	if (ItemCount <= 0 || ItemLevel <= 0) {
		UE_LOG(LogActionRPG, Error, TEXT("ARPGPlayerControllerBase.AddInventoryItem Error: ItemCout or ItemLevel is Illegal, ItemName: %s, ItemCount: %d, ItemLevel: %d"), *NewItem->GetName(), ItemCount, ItemLevel);
		return false;
	}

	bool bChanged = false;
	FRPGItemData OldData;
	GetInventoryItemData(NewItem, OldData);
	FRPGItemData NewData = OldData;
	NewData.UpdateItemData(FRPGItemData(ItemCount, ItemLevel), NewItem->MaxCount, NewItem->MaxLevel);;
	if (NewData != OldData) {
		InventoryData.Add(NewItem, NewData);
		NotifyInventoryItemChanged(true, NewItem);
		bChanged = true;
	}
	if (bAutoSlot) {
		bChanged |= FillEmptySlotWithItem(NewItem);
	}
	if (bChanged) {
		SaveInventory();
		return true;
	}
	return false;
}

bool ARPGPlayerControllerBase::RemoveInventoryItem(URPGItem* RemovedItem, int32 RemoveCount)
{
	if (!RemovedItem) {
		UE_LOG(LogActionRPG, Error, TEXT("URPGPlayerControllerBase.RemoveInventory Error: RemoveItem is nullpter"));
		return false;
	}
	FRPGItemData NewData;
	GetInventoryItemData(RemovedItem, NewData);
	if (!NewData.IsValid()) {
		UE_LOG(LogActionRPG, Error, TEXT("URPGPlayerControllerBase.RemoveInventory Error: Not Found RemovedItem in Inventory, RemovedItem name is: %s"), *RemovedItem->GetName());
		return false;
	}
	if (RemoveCount <= 0) { // 如果删除的数量小于等于0，则全删除
		NewData.Count = 0;
	}
	else {
		NewData.Count -= RemoveCount;
	}

	if (NewData.Count > 0) { // 如果还有余量，设置到背包中，否则将物品从背包和插槽中删除
		InventoryData.Add(RemovedItem, NewData);
	}
	else {
		InventoryData.Remove(RemovedItem);
		for (TPair<FRPGItemSlot, URPGItem*> Pair : SlottedItems) {
			if (Pair.Value == RemovedItem) {
				Pair.Value = nullptr;
				NotifySlottedItemChanged(Pair.Key, Pair.Value);
			}
		}
	}

	NotifyInventoryItemChanged(false, RemovedItem);
	SaveInventory();
	return true;
}

void ARPGPlayerControllerBase::GetInventoryItems(TArray<URPGItem*>& Items, FPrimaryAssetType ItemType) const
{
	for (const TPair<URPGItem*, FRPGItemData>& Pair : InventoryData) {
		if (Pair.Key) {
			if (ItemType == Pair.Key->ItemType || !ItemType.IsValid()) {
				Items.Add(Pair.Key);
			}
		}
	}
}

int32 ARPGPlayerControllerBase::GetInventoryItemCount(URPGItem* Item) const
{
	const FRPGItemData* ItemData = InventoryData.Find(Item);
	if (ItemData) {
		return ItemData->Count;
	}
	return 0;
}

bool ARPGPlayerControllerBase::GetInventoryItemData(URPGItem* Item, FRPGItemData& ItemData) const
{
	const FRPGItemData* Found = InventoryData.Find(Item);
	if (Found) {
		ItemData = *Found;
		return true;
	}
	else {
		ItemData = FRPGItemData(0, 0);
		return false;
	}
}

bool ARPGPlayerControllerBase::SetSlottedItem(FRPGItemSlot ItemSlot, URPGItem* Item)
{
	bool bFound = false;
	for (TPair<FRPGItemSlot, URPGItem*> Pair : SlottedItems) {
		if (Pair.Key == ItemSlot) {
			bFound = true;
			Pair.Value = Item;
			NotifySlottedItemChanged(Pair.Key, Pair.Value);;
		}
		else if (Item != nullptr && Pair.Value == Item) { // 找到Key不同，但值相同的插槽，清除之
			Pair.Value = nullptr;
			NotifySlottedItemChanged(Pair.Key, Pair.Value);;
		}
	}
	if (bFound) {
		SaveInventory();
		return true;
	}
	
	return false;
}

URPGItem* ARPGPlayerControllerBase::GetSlottedItem(FRPGItemSlot ItemSlot) const
{
	URPGItem* const* Item = SlottedItems.Find(ItemSlot);
	if (Item) {
		return *Item;
	}
	return nullptr;
}

void ARPGPlayerControllerBase::GetSlottedItems(TArray<URPGItem*>& Items, FPrimaryAssetType ItemType) const
{
	for (const TPair<FRPGItemSlot, URPGItem*> Pair : SlottedItems) {
		if (Pair.Key.ItemType == ItemType || !ItemType.IsValid()) {
			Items.Add(Pair.Value);
		}
	}
}

void ARPGPlayerControllerBase::FillEmptySlots()
{
	bool bShouldSave = false;
	for (TPair<URPGItem*, FRPGItemData> Pair : InventoryData) {
		bShouldSave |= FillEmptySlotWithItem(Pair.Key);;
	}

	if (bShouldSave) {
		SaveInventory();
	}
}

bool ARPGPlayerControllerBase::SaveInventory()
{
	UWorld* World = GetWorld();
	URPGGameInstanceBase* RPGGameInstance = World ? World->GetGameInstance<URPGGameInstanceBase>() : nullptr;
	if (!RPGGameInstance) {
		return false;
	}
	URPGSaveGame* CurrentSaveGame = RPGGameInstance->GetCurrentSaveGame();
	if (CurrentSaveGame) {
		CurrentSaveGame->InventoryData.Reset();
		CurrentSaveGame->SlotItems.Reset();

		for (const TPair<URPGItem*, FRPGItemData>& Pair : InventoryData) {// 将背包数据同步到SaveGame的InventoryData中
			if (Pair.Key) {
				CurrentSaveGame->InventoryData.Add(Pair.Key->GetPrimaryAssetId(), Pair.Value);
			}
		}

		for (const TPair<FRPGItemSlot, URPGItem*>& Pair : SlottedItems) { // 将背包中的插槽数据同步到SaveGame的SlotItems中
			FPrimaryAssetId AssetId;
			if (Pair.Value) {
				AssetId = Pair.Value->GetPrimaryAssetId();
			}
			CurrentSaveGame->SlotItems.Add(Pair.Key, AssetId);
		}
		
		RPGGameInstance->WriteSaveGame();
		return true;
	}
	return false;
}

bool ARPGPlayerControllerBase::LoadInventory()
{
	InventoryData.Reset();
	SlottedItems.Reset();
	UWorld* World = GetWorld();
	URPGGameInstanceBase* RPGGameIntance = World ? World->GetGameInstance<URPGGameInstanceBase>() : nullptr;
	if (!RPGGameIntance) {
		return false;
	}

	if (!RPGGameIntance->OnSaveGameLoadedNativeDelegate.IsBoundToObject(this)) {// 如果没有GameInstance绑定HandleSaveGame事件，在这里进行绑定
		RPGGameIntance->OnSaveGameLoadedNativeDelegate.AddUObject(this,&ARPGPlayerControllerBase::HandleSaveGameLoad);
	}

	for (TPair<FPrimaryAssetType, int32> Pair : RPGGameIntance->ItemSlotPerType) { // 使用GameIntance中默认定义的插槽来初始化背包插槽数据
		for (int32 SlotNumber = 0; SlotNumber < Pair.Value; SlotNumber++) {
			SlottedItems.Add(FRPGItemSlot(Pair.Key, SlotNumber), nullptr);
		}
	}

	URPGSaveGame* CurrentSaveGame = RPGGameIntance->GetCurrentSaveGame();
	URPGAssetManager& RPGAssetManager = URPGAssetManager::Get();
	if (CurrentSaveGame) {
		for (const TPair<FPrimaryAssetId, FRPGItemData>& Pair : CurrentSaveGame->InventoryData) {// 同步物品数据
			URPGItem* LoadedItem = RPGAssetManager.ForceLoadItem(Pair.Key);
			if (LoadedItem) {
				InventoryData.Add(LoadedItem, Pair.Value);
			}
		}

		bool bFoundAnySlot = false; // 同步插槽数据
		for (const TPair<FRPGItemSlot, FPrimaryAssetId>& Pair : CurrentSaveGame->SlotItems) {
			URPGItem* LoadedItem = RPGAssetManager.ForceLoadItem(Pair.Value);
			if (LoadedItem) {
				SlottedItems.Add(Pair.Key, LoadedItem);
				bFoundAnySlot = true;
			}
		}

		if (!bFoundAnySlot) { // 如果没有找到任何合适的插槽，这里做一下默认的插槽数据填充，如果插槽有数据，这里就不做了
			FillEmptySlots();
		}
		NotifyInventoryLoaded();
		return true;
	}

	NotifyInventoryLoaded(); // 即使GameSave没有，也要发布背包加载完成的通知，用于通知UI(	 去检验通知UI的逻辑)
	return false;
}

bool ARPGPlayerControllerBase::FillEmptySlotWithItem(URPGItem* NewItem)
{
	FPrimaryAssetType NewItemType = NewItem->ItemType;
	FRPGItemSlot EmptySlot;
	for (TPair<FRPGItemSlot, URPGItem*>& Pair : SlottedItems) {
		if (Pair.Key.ItemType == NewItemType) {
			if (Pair.Value == NewItem) { // 如果插槽中已经有该物品
				return false;
			}
			else if (Pair.Value == nullptr && (!EmptySlot.IsValid() || Pair.Key.SlotNumber < EmptySlot.SlotNumber)) { // 如果该插槽的物品为空且EmptySlot还没有值，则将此插槽付给EmptySlot,并且如果后面存储插槽序号比当前EmptySlot还小的，让EmptySlot等于较小的那个，这样做的目的在于尽量让物品往前靠（插槽序号越小，插槽越靠前）
				EmptySlot = Pair.Key;
			}
		}
	}
	if (EmptySlot.IsValid()) {
		SlottedItems[EmptySlot] = NewItem;
		NotifySlottedItemChanged(EmptySlot, NewItem);
		return true;
	}
	return false;
}

void ARPGPlayerControllerBase::NotifyInventoryItemChanged(bool bAdded, URPGItem* Item)
{
	OnInventoryItemChangedNativeDelegate.Broadcast(bAdded, Item);
	OnInventoryItemChangedDelegate.Broadcast(bAdded, Item); // 先通知本地c++代码，再通知蓝图

	InventoryItemChanged(bAdded, Item);
}

void ARPGPlayerControllerBase::NotifySlottedItemChanged(FRPGItemSlot ItemSlot, URPGItem* Item)
{
	OnSlotItemChangedNativeDelegate.Broadcast(ItemSlot, Item);
	OnSlotItemChangedDelegate.Broadcast(ItemSlot, Item); // 先通知本地c++代码，再通知蓝图

	SlottedItemChanged(ItemSlot, Item);
}

void ARPGPlayerControllerBase::NotifyInventoryLoaded()
{
	OnInventoryLoadedNativeDelegate.Broadcast();
	OnInventoryLoadedDelegate.Broadcast(); // 先通知本地c++代码，再通知蓝图
}

void ARPGPlayerControllerBase::HandleSaveGameLoad(URPGSaveGame* SaveGame)
{

	LoadInventory();
}
