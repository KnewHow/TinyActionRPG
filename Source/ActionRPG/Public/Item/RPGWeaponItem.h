#pragma once

/**
* 武器的C++基类
*/

#include "RPGItem.h"
#include "RPGWeaponItem.generated.h"

UCLASS()
class URPGWeaponItem : public URPGItem {
	GENERATED_BODY()
public:
	URPGWeaponItem() {
		ItemType = URPGAssetManager::WeaponItemType;
	}

	/**
	对应的武器类型
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
		TSubclassOf<AActor> WeaponActor;
};