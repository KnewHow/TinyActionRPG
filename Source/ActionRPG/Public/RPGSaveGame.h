#pragma once

/**
* �Զ����SaveGame,���ڴ浵����
*/
#include "ActionRPG.h"
#include "RPGTypes.h"
#include "GameFramework/SaveGame.h"
#include "RPGSaveGame.generated.h"

/**
* 
* ����һд����SaveGame�İ汾�ţ�
* ���ص�c++���뽫��Բ�ͬ�İ汾����ͬ�Ĵ������һ�����һЩ�汾��
* ���˾�������ڱ���Ŀ�к����Ƕ���ģ���ȫû���ã���
*/
namespace ERPGSaveGameVersion {
	enum type
	{
		Initial, // ��ʼ�汾
		AddedInventory, // �����Ѿ��������������
		AddedItemData, // ����� ItemData 
		//-------------------������µİ汾������Ҫ�ڴ˺���֮ǰ��ӣ���������ڴ˺���֮��-------------------------------
		VersionPlusOne,
		LatestVersion = VersionPlusOne - 1 // ���°汾
	};
}

UCLASS(BlueprintType)
class ACTIONRPG_API URPGSaveGame : public USaveGame {
	
	GENERATED_BODY()

public:
	URPGSaveGame();

	/**
	* ��Ʒ����Ʒ����֮���ӳ�䣬���Է�Ӧÿ����Ʒ�ĵȼ�������
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveGame)
		TMap<FPrimaryAssetId, FRPGItemData> InventoryData;

	/**
	* ��ۺ���Ʒ��ӳ��
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveGame)
		TMap<FRPGItemSlot, FPrimaryAssetId> SlotItems;

	/**
	* �û���ΨһID
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveGame)
		FString UserId;

protected:
	/**
	* һЩĬ�ϵ���Ʒ������ֻ�����ȡ��
	* Ȼ���Ҿ������������ȫû��
	* TODO: ���������
	*/
	UPROPERTY()
		TArray<FPrimaryAssetId> InventoryItems_DEPRECATED;
	
	/**
	* �汾�ţ��������������İ汾���е�һ��
	*/
	UPROPERTY()
		int32 SaveDataVersion;

	/**
	* ���л������������޸��汾��Ĭ�϶�ȡ�����˾���û��ʲô�ã���Ϊ����ִ�в������ĵĴ���
	* TODO: ���������
	* 
	*/
	virtual void Serialize(FArchive& Ar) override;
};