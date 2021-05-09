#pragma once

/**
* 药水资源的C++基类，他应该在蓝图中被实例化
*/

#include "RPGItem.h"
#include "RPGPotionItem.generated.h"

UCLASS()
class URPGPotionItem : public URPGItem {
	GENERATED_BODY()
public:
	/**
	* 构造函数初始化类型 
	*/
	URPGPotionItem() {
		ItemType = URPGAssetManager::PotionItemType;
	}
};
