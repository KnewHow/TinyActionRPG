#include "Item/RPGItem.h"

bool URPGItem::IsConsumable()
{
	return MaxCount <= 0;
}

FString URPGItem::GetIdentifierString()
{
	return GetPrimaryAssetId().ToString();
}

FPrimaryAssetId URPGItem::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(ItemType, GetFName());
}
