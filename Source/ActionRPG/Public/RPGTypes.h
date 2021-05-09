#pragma once

//-----------------------------------------------
// ��ͷ�ļ�����ȫ�ֵ�ö�ٺ�һЩ���õ������࣬��Щ�������Ա��������ͼʹ��
// ����Щȫ��ͨ�õ����ͼ��е�һ��������ͷ�ļ��п��Լ���ѭ�����õ�����
// ������ԣ���Ҫ���԰�`���ݱ�`�Ľṹ���ͷŵ�����
//-----------------------------------------------


#include "UObject/PrimaryAssetId.h"
#include "RPGTypes.generated.h"

class URPGItem;
class URPGSaveGame;

/**
* ��Ʒ�����Ϣ�����ڱ�UI��ʾ
*/
USTRUCT(BlueprintType)
struct ACTIONRPG_API FRPGItemSlot {
	
	GENERATED_BODY()

	/**
	* Ĭ�Ϲ��캯���������Ĭ��Ϊ-1
	*/
	FRPGItemSlot() 
		: SlotNumber(-1)
	{}

	/**
	* ���캯��
	* @param InItemType ��Ʒ����
	* @param InSlotNumber �������
	*/
	FRPGItemSlot(const FPrimaryAssetType& InItemType, int32 InSlotNumber)
		: ItemType(InItemType), SlotNumber(InSlotNumber)
	{}


	/**
	* �������Ʒ������
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	FPrimaryAssetType ItemType;

	/**
	* �������
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	int32 SlotNumber;

	bool operator==(const FRPGItemSlot& other) const {
		return ItemType == other.ItemType && SlotNumber == other.SlotNumber;
	}

	bool operator!=(const FRPGItemSlot& other) const {
		return !(*this == other);
	}

	/**
	* Ϊ���� FRPGItemSlot ��Map/Set ����Hashֵ��ʱ������㣬����ʵ�ִ˷���
	*/
	friend inline uint32 GetTypeHash(const FRPGItemSlot& Key) {
		uint32 Hash = 0;
		Hash = HashCombine(Hash, GetTypeHash(Key.ItemType));;
		Hash = HashCombine(Hash, (uint32)Key.SlotNumber);
		return Hash;
	}

	/**
	* �ò���Ƿ�Ϸ�
	*/
	bool IsValid() const {
		return ItemType.IsValid() && SlotNumber >= 0;
	}
};

/**
* �����е���Ʒ��Ϣ��������Ʒ�������͵ȼ�
*/
USTRUCT(BlueprintType)
struct ACTIONRPG_API FRPGItemData {
	GENERATED_BODY()
	
	FRPGItemData()
	:Count(1), Level(1)
	{}

	FRPGItemData(int32 Counter, int32 Level)
		:Count(Counter), Level(Level)
	{}
	/**
	* ��Ʒ�ڱ����е�����������С��1
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
		int32 Count;

	/**
	* ��Ʒ�ȼ����˵ȼ����ᱻ����ʵ��������
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
		int32 Level;


	bool operator==(const FRPGItemData& Other) const {
		return Count == Other.Count && Level == Other.Count;
	}

	bool operator!=(const FRPGItemData& Other) const {
		return !(*this == Other);
	}

	bool IsValid() const {
		return Count > 0;
	}

	/**
	* ��������
	* @param Other ��Ҫ���µ�����
	* @param MaxCount �����������
	* @param MaxLevel ���ȼ�����
	*/
	void UpdateItemData(const FRPGItemData& Other, int32 MaxCount, int32 MaxLevel) {
		// ����������С�ڵ���0������ΪINT32_MAX��������
		if (MaxCount <= 0) {
			MaxCount = INT32_MAX;
		}
		if (MaxLevel <= 0) {
			MaxLevel = INT32_MAX;
		}

		Count = FMath::Clamp(Count + Other.Count, 1, MaxCount);
		Level = FMath::Clamp(Level + Other.Level, 1, MaxCount);
	}
};


//------------------------------------------------
// ������һЩ�Ự������
//------------------------------------------------

// ������Ʒ�ı�Ự
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryItemChanged, bool, bAdded, URPGItem*, Item);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInventoryItemChangedNative, bool, URPGItem*);

// ������۸ı�Ự
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSlotItemChanged, FRPGItemSlot, ItemSlot, URPGItem*, Item);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSlotItemChangedNative, FRPGItemSlot, URPGItem*);

// ����������ɻỰ
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryLoaded);
DECLARE_MULTICAST_DELEGATE(FOnInventoryLoadedNative);

// SaveGame ������ɻỰ
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveGameLoaded, URPGSaveGame*, SaveGame);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSaveGameLoadedNative, URPGSaveGame*);