#pragma once

/**
* 所有物品的基类
*/

#include "../ActionRPG.h"
#include "../RPGAssetManager.h"
#include "Engine/DataAsset.h"
#include "Styling/SlateBrush.h"
#include "UObject/PrimaryAssetId.h"
#include "RPGItem.generated.h"

class URPGGameplayAbility;

UCLASS(Abstract, BlueprintType)
class ACTIONRPG_API URPGItem: public UPrimaryDataAsset {
	
	GENERATED_BODY()

public:
	/**
	* 商品的资源类型，实现该类的c++子类需要预先设置好此类型
	*/
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = Item)
		FPrimaryAssetType ItemType;

	/**
	* 商品名称(玩家可见)
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
		FText ItemName;

	/**
	* 商品描述(用户可见)
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
		FText ItemDescription;
	
	/**
	* 商品图标
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
		FSlateBrush ItemIcon;

	/**
	* 商品价格
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
		int32 Price;

	/**
	* 背包中可以放置的最大数量, 如果该值 <= 0,代表无限制
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Max)
		int32 MaxCount;
	
	/**
	* 物品最大等级， <= 0 意味着无限制
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Max)
		int32 MaxLevel;


	/**
	* 该商品给予玩家的能力
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities)
		TSubclassOf<URPGGameplayAbility> GrantedAbility;

	/**
	* 该能力的等级，如果小于<=0,则与玩家等级对应
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities)
		int32 AbilityLevel;

	/**
	* 该商品是否是可消费的，我们使用 MaxCount <= 0 来标记
	* 如果该商品的 MaxCount <= 0, 表示可消费。
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Max)
		bool IsConsumable();

	/**
	* 返回该商品的逻辑名称，等价于该商品的原始资源ID
	*/
	UFUNCTION(BlueprintCallable, Category = Item)
		FString GetIdentifierString();

	/**
	* 获取原始的资源ID,重载此函数用于保存类型
	*/
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
