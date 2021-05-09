#pragma once
#include "ActionRPG.h"
#include "RPGInventoryInterface.generated.h"

/**
* һ���ͱ�����صĽӿڣ�ʵ������Ҫ������µķ���
*/

UINTERFACE(MinimalAPI, meta=(CannotImplementInterfaceInBlueprint))
class URPGInventoryInterface : public UInterface {
	GENERATED_BODY()
};

class ACTIONRPG_API IRPGInventoryInterface {
	GENERATED_BODY()
public:
	
	/**
	* ��ȡ��Ʒ����Ʒ���ݵ�ӳ��
	* @return ����һ��Map��ӳ��
	*/
	virtual const TMap<URPGItem*, FRPGItemData>& GetInventoryDataMap() const = 0;

	/**
	* ��ȡ��۵���Ʒ��ӳ��
	* @return ��۵���Ʒ��ӳ��
	*/
	virtual const TMap<FRPGItemSlot, URPGItem*>& GetSlottedItemMap() const = 0;

	/**
	* ��ȡ������Ʒ�ı�ı��ػỰ
	* @return ����һ�����ػỰ
	*/
	virtual FOnInventoryItemChangedNative& GetInventoryItemChangedDelegate() = 0;

	/**
	* ��ȡ��۸ı䱾�ػỰ
	* @param ����һ�����ػỰ
	*/
	virtual FOnSlotItemChangedNative& GetSlotItemChangedDelegate() = 0;

	/**
	* ��ȡ����������ɵı��ػỰ
	* @param ����һ�����ػỰ
	*/
	virtual FOnInventoryLoadedNative& GetInventoryLoadedDelegate() = 0;

};