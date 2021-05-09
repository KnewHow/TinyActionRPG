#pragma once
#include "ActionRPG.h"
#include "RPGTypes.h"
#include "Engine/GameInstance.h"
#include "RPGGameInstanceBase.generated.h"

/**
* RPGGameInstance ���࣬������һϵ��Ĭ�ϵĲ�����
* �����ʼ�����������Դ���ͺ��������Լ�ÿ����Դ�Ĳ��������һЩ�ͼ���SaveGame�����Ժͷ���
*/

UCLASS()
class ACTIONRPG_API URPGGameInstanceBase : public UGameInstance {
	GENERATED_BODY()
public:
	URPGGameInstanceBase();

	/**
	* Ĭ�ϵı������ݣ�����ÿ����Դ��Ӧ������
	*/
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = Inventory)
		TMap<FPrimaryAssetId, FRPGItemData> DefaultInventory;

	/**
	* ÿ����Դ��ӵ�в�۵���������GameInstance��Ԥ�ȶ��壬���ں�����ʼ����������
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
		TMap<FPrimaryAssetType, int32> ItemSlotPerType;

	/**
	* ���� SaveGame �Ĳ������
	*/
	UPROPERTY(BlueprintReadWrite, Category = Save)
		FString SaveSlot;

	/**
	* ����SaveGame�� user index
	*/
	UPROPERTY(BlueprintReadWrite, Category = Save)
		int32 SaveUserIndex;

	/**
	* SaveGame �Ѽ��ػỰ��������ͼ����
	*/
	UPROPERTY(BlueprintAssignable, Category = Inventory)
		FOnSaveGameLoaded OnSaveGameLoadedDelegate;

	/**
	* SaveGame �Ѽ��صı��ػỰ
	*/
	FOnSaveGameLoadedNative OnSaveGameLoadedNativeDelegate;

	/**
	* ��SaveGame�����Ĭ�ϵı������ݣ�������Ҫ��֤SaveGame����Ϊnullpter
	* @param SaveGame ��������
	* @param bRemoveExtra �Ƿ�����ձ����е�Ĭ������
	*/
	UFUNCTION(BlueprintCallable, Category = Inventory)
		void AddDefaultInventory(URPGSaveGame* SaveGame, bool bRemoveExtra = false);

	/**
	* �ж��Ƿ��ǺϷ��Ĳ��
	* @param ItemSlot �����Ϣ
	* @return true Ϊ��Ч������Ϊ��Ч
	*/
	UFUNCTION(BlueprintCallable, Category = Inventory)
		bool isValidItemSlot(FRPGItemSlot ItemSlot) const;
	
	/**
	* ��ȡ��ǰ��SaveGame
	* @return ���ص�ǰSaveGame��ָ��
	*/
	UFUNCTION(BlueprintCallable, Category = Save)
		URPGSaveGame* GetCurrentSaveGame();

	/**
	* ���õ�ǰSaveGame��״̬Ϊ�Ƿ� �ɱ���Ϳɼ���
	*/
	UFUNCTION(BlueprintCallable, Category = Save)
		void SetSavingEnabled(bool bEnabled);

	/**
	* ���ػ����½�һ��SaveGame,���Ҷ�SaveGame��һЩ�����˷����Ե���[HandleSaveGameLoaded] ����ʼ�����ػ��ߴ�����SaveGame
	* ��������д���SaveGame,�ͼ��أ�����ʹ������˷������õļ���SaveGame�ķ�����ͬ������˿��ܻ�����߳�����
	* @return ����ǴӴ��̼��ص�SaveGame����true,����Ǵ����ķ��� false
	*/
	UFUNCTION(BlueprintCallable, Category = Save)
		bool LoadOrCreateSaveGame();

	/**
	* ����Ӵ����м��ص�SaveGame���˷�����ʹ��Ĭ�ϵı���������ѡ���ʼ��SaveGame
	* ���SaveGame���Ѿ�������Ĭ�ϵı����������������������Ĭ�ϵı���������ʼ��SaveGame
	* ���ڲ����е� SaveGame ����Ϊ�գ���˵� SaveGameΪ��ʱ�����������½�һ��SaveGame����Ȼ���ñ���Ĭ����������ʼ����
	* �˷�����㲥[OnSaveGameLoaded]��][OnSaveGameLoadedNative]�Ự
	* @return bool ���SaveGame��Ϊnullpter������true,���򷵻�false
	*/
	UFUNCTION(BlueprintCallable, Category = Save)
		bool HandleSaveGameLoaded(USaveGame* SaveGame);

	/**
	* ��ȡ�洢�Ĳ����Ϣ�������������(SlotName)���û�����(UserIndex)
	* @param SlotName ���صĲ������
	* @param UserIndex ���ص��û�����
	*/
	UFUNCTION(BlueprintCallable, Category = Save)
		void GetSaveSlotInfo(FString& SlotName, int32& UserIndex) const;

	/**
	* ��SaveGameд����̣��˷�����ͨ���첽�ķ�ʽ��SaveGameд�����
	* @return ���д��ɹ�����true�����򷵻�false
	*/
	UFUNCTION(BlueprintCallable, Category = Save)
		bool WriteSaveGame();

	/**
	* ����SaveGame����ʹ��Ĭ�ϵı�����������ʼ��SaveGame�е�����
	* �˷�����㲥[OnSaveGameLoaded]��][OnSaveGameLoadedNative]�Ự
	*/
	UFUNCTION(BlueprintCallable, Category = Save)
		void ResetSaveGame();

protected:
	/**
	* ��ǰ��SaveGame
	*/
	UPROPERTY()
		URPGSaveGame* CurrentSaveGame;

	/**
	* �Ƿ�SaveGame���Ա����浽������
	*/
	UPROPERTY()
		bool bSavingEnabled;
	
	/**
	* ��ǰ�Ƿ����ڱ����У����ڱ����Ŷ�
	*/
	UPROPERTY()
		bool bCurrentlySaving;

	/**
	* ��ǰ�Ƿ����ύ��������󣬴˲������ڵ�������������������ʱ
	* ���ǻ��ȴ�������һ������bPendingSaveRequested��Ϊtrue,�������һ�α�����ٴ�ִ�б��棨�˲����ڻص�����HandleAsyncSaveִ�У�
	*/
	UPROPERTY()
		bool bPendingSaveRequested;

	/**
	* �첽д����̳ɹ���Ļص�����
	*/
	virtual void HandleAsyncSave(const FString& SlotName, const int32 UserIndex, bool bSuccess);
};