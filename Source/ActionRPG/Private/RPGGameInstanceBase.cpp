#include "RPGGameInstanceBase.h"
#include "RPGSaveGame.h"
URPGGameInstanceBase::URPGGameInstanceBase():
	SaveSlot("SaveGame"), SaveUserIndex(0) { // init SaveSlot and SaveUserIndex default values
}

void URPGGameInstanceBase::AddDefaultInventory(URPGSaveGame* SaveGame, bool bRemoveExtra)
{
	if (bRemoveExtra) {
		SaveGame->InventoryData.Reset();
	}

	for (const TPair<FPrimaryAssetId, FRPGItemData>& Pair : DefaultInventory) {
		if (!SaveGame->InventoryData.Contains(Pair.Key)) {
			SaveGame->InventoryData.Add(Pair);
		}
	}
}

bool URPGGameInstanceBase::isValidItemSlot(FRPGItemSlot ItemSlot) const
{
	if (ItemSlot.IsValid()) {
		const int32* FoundCount = ItemSlotPerType.Find(ItemSlot.ItemType);
		if (FoundCount && ItemSlot.SlotNumber < *FoundCount) {
			return true;
		}
	}
	return false;
}

URPGSaveGame* URPGGameInstanceBase::GetCurrentSaveGame()
{
	return this->CurrentSaveGame;
}

void URPGGameInstanceBase::SetSavingEnabled(bool bEnabled)
{
	this->bSavingEnabled = bEnabled;
}

bool URPGGameInstanceBase::LoadOrCreateSaveGame()
{
	URPGSaveGame* SaveGame = nullptr;
	if (UGameplayStatics::DoesSaveGameExist(SaveSlot, SaveUserIndex) && bSavingEnabled) {
		SaveGame = Cast<URPGSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlot, SaveUserIndex));
	}
	return HandleSaveGameLoaded(SaveGame);
}

bool URPGGameInstanceBase::HandleSaveGameLoaded(USaveGame* SaveGame)
{
	bool bLoaded = false;
	if (!bSavingEnabled) {
		SaveGame = nullptr;
	}
	CurrentSaveGame = Cast<URPGSaveGame>(SaveGame);
	if (CurrentSaveGame) {
		AddDefaultInventory(CurrentSaveGame, false);
		bLoaded = true;
	}
	else {
		CurrentSaveGame = Cast<URPGSaveGame>(UGameplayStatics::CreateSaveGameObject(URPGSaveGame::StaticClass()));
		AddDefaultInventory(CurrentSaveGame, true);
	}
	OnSaveGameLoadedDelegate.Broadcast(CurrentSaveGame);
	OnSaveGameLoadedNativeDelegate.Broadcast(CurrentSaveGame);
	return bLoaded;
}

void URPGGameInstanceBase::GetSaveSlotInfo(FString& SlotName, int32& UserIndex) const
{
	SlotName = SaveSlot;
	UserIndex = SaveUserIndex;
}

bool URPGGameInstanceBase::WriteSaveGame()
{
	if (bSavingEnabled) {
		if (bCurrentlySaving) {// 如果当前正在保存，记录此次请求
			bPendingSaveRequested = true;
			return true;
		}
		else {
			bCurrentlySaving = true;
			UGameplayStatics::AsyncSaveGameToSlot(
				GetCurrentSaveGame(), 
				SaveSlot, SaveUserIndex, 
				FAsyncSaveGameToSlotDelegate::CreateUObject(this, &URPGGameInstanceBase::HandleAsyncSave)
			);
			return true;
		}
	}
	return false;
}

void URPGGameInstanceBase::ResetSaveGame()
{
	HandleSaveGameLoaded(nullptr);
}

void URPGGameInstanceBase::HandleAsyncSave(const FString& SlotName, const int32 UserIndex, bool bSuccess)
{
	ensure(bCurrentlySaving);
	bCurrentlySaving = false;
	if (bPendingSaveRequested) {
		WriteSaveGame();
		bPendingSaveRequested = false;
	}
}
