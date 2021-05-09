#pragma once
/**
* 金币的C++基类
*/

#include "RPGItem.h"
#include "RPGTokenItem.generated.h"

UCLASS()
class URPGTokenItem : public URPGItem {
	GENERATED_BODY()
public:
	URPGTokenItem() {
		ItemType = URPGAssetManager::TokenItemType;
	}
};