#pragma once

#include "RPGItem.h"
#include "RPGSkillItem.generated.h"

UCLASS()
class URPGSkillItem : public URPGItem {
	GENERATED_BODY()
public:
	/**
	* 默认构造函数初始化类型
	*/
	URPGSkillItem() {
		ItemType = URPGAssetManager::SkillItemType;
	}
};