#pragma once

#include "RPGItem.h"
#include "RPGSkillItem.generated.h"

UCLASS()
class URPGSkillItem : public URPGItem {
	GENERATED_BODY()
public:
	/**
	* Ĭ�Ϲ��캯����ʼ������
	*/
	URPGSkillItem() {
		ItemType = URPGAssetManager::SkillItemType;
	}
};