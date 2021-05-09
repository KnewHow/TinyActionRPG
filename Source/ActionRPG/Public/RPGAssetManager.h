#pragma once

/**
* �Զ�����Դ�����������ڼ����Զ������Ʒ��Դ��
* ���Ҷ����˸�����Ʒ��Դ����
*/

#include "Engine/AssetManager.h"
#include "ActionRPG.h"
#include "RPGAssetManager.generated.h"

class URPGItem;

UCLASS()
class ACTIONRPG_API URPGAssetManager : public UAssetManager {
	GENERATED_BODY()
public:
	/**
	* Ĭ�Ϲ��캯��
	*/
	URPGAssetManager() {};
	/**
	* ��ʼ��ʼ������
	*/
	virtual void StartInitialLoading() override;

	/**
	* �����ﶨ��������Ʒ������
	*/
	static const FPrimaryAssetType PotionItemType; // ҩƷ����
	static const FPrimaryAssetType SkillItemType; // ��������
	static const FPrimaryAssetType TokenItemType; // �������
	static const FPrimaryAssetType WeaponItemType; // ��������

	/**
	* ��ȡ��ǰ����Դ������
	*/
	static URPGAssetManager& Get();
	
	/**
	* ������ԴIDͬ��������Դ���˷������ܻ����������Ƕ��ڲ��ܵȴ�����Դ���Ƿǳ����õ�
	* @param PrimaryAssetId ��Դ��Ψһ��ʶ��
	* @param bLogWarning �Ƿ���Ҫ��¼��־
	*/
	URPGItem* ForceLoadItem(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning = false);
};