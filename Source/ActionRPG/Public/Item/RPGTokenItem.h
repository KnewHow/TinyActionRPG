#pragma once
/**
* ��ҵ�C++����
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