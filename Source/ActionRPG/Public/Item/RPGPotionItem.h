#pragma once

/**
* ҩˮ��Դ��C++���࣬��Ӧ������ͼ�б�ʵ����
*/

#include "RPGItem.h"
#include "RPGPotionItem.generated.h"

UCLASS()
class URPGPotionItem : public URPGItem {
	GENERATED_BODY()
public:
	/**
	* ���캯����ʼ������ 
	*/
	URPGPotionItem() {
		ItemType = URPGAssetManager::PotionItemType;
	}
};
