#pragma once
#include "ActionRPG.h"
#include "RPGTypes.h"
#include "RPGInventoryInterface.h"
#include "Abilities/RPGAbilitySystemComponent.h"
#include "Abilities/RPGAttributeSet.h"
#include "Abilities/RPGGameplayAbility.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "RPGCharacterBase.generated.h"

/**
* 角色类型的枚举
*/
UENUM()
enum class UCharacterType :uint8 {
	Player = 1, // 玩家
	Emeny = 2 // 敌人
};

/**
* RPG Character 基类，里面主要负责初始化角色的能力、角色能力的添加和删除等基本操作
*/
UCLASS()
class ACTIONRPG_API ARPGCharacterBase : public ACharacter, public IAbilitySystemInterface {
	GENERATED_BODY()
public:
	ARPGCharacterBase();

	/**
	* 当控制器控制角色的时候开始执行
	* @param NewController 新的控制器
	*/
	virtual void PossessedBy(AController* NewController) override;

	/**
	* 当控制器取消控制角色的时候开始执行
	*/
	virtual void UnPossessed() override;

	/**
	* 当控制器被复制时调用
	*/
	virtual void OnRep_Controller() override;

	/**
	* 获取能力系统组件
	*/
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/**
	* 处理需要被复制的属性，在此类中，只有CharacterLevel这一个需要被复制的属性
	*/
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const;
	
	/**
	* 获取角色当前生命值
	*/
	UFUNCTION(BlueprintCallable)
		virtual float GetHealth() const;

	/**
	* 获取角色最大生命值上限
	*/
	UFUNCTION(BlueprintCallable)
		virtual float GetMaxHealth() const;

	/**
	* 获取角色当前法力值
	*/
	UFUNCTION(BlueprintCallable)
		virtual float GetMana() const;

	/**
	* 获取最大法力值上限
	*/
	UFUNCTION(BlueprintCallable)
		virtual float GetMaxMana() const;

	/**
	* 获取造成的伤害
	*/
	UFUNCTION(BlueprintCallable)
		virtual float GetDamage() const;

	/**
	* 获取移动速度
	*/
	UFUNCTION(BlueprintCallable)
		virtual float GetMoveSpeed() const;

	/**
	* 获取角色等级
	*/
	UFUNCTION(BlueprintCallable)
	virtual int32 GetCharacterLevel() const;

	/**
	* 设置角色等级
	* @param NewLevel 新的角色等级
	* @return 设置成功返回true，否则返回false
	*/
	UFUNCTION(BlueprintCallable)
	virtual bool SetCharacterLevel(int32 NewLevel);

	/**
	* 获取角色类型
	*/
	UFUNCTION(BlueprintCallable)
	virtual UCharacterType GetCharacterType() const;

	/**
	* 激活指定插槽中的能力
	* @param ItemSlot 插槽
	* @param bAllowRemoteActivation 是否允许远程激活
	* @return 如果激活成功，则返回true,否则返回false.
	*/
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool ActivateAbilitiesWithItemSlot(const FRPGItemSlot& ItemSlot, bool bAllowRemoteActivation = true);

	/**
	* 获取插槽中可激活的能力
	* @param ItemSlot 插槽
	* @param ActiveAbilities 需要返回的能力
	*/
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void GetActiveAbilitiesWithItemSlot(const FRPGItemSlot& ItemSlot, TArray<URPGGameplayAbility*>& ActiveAbilities);

	/**
	* 根据标签来激活能力
	* @param AbilityTags 能力标签
	* @param bAllowRemoteActivation 是否允许远程激活
	* @return 激活成功返回true,否则返回false
	*/
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		bool ActivateAbilitiesWithTags(const FGameplayTagContainer& AbilityTags, bool bAllowRemoteActivation = true);

	/**
	* 根据标签获取可激活的能力
	*/
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		void GetActiveAbilitiesWithTags(const FGameplayTagContainer& AbilityTags, TArray<URPGGameplayAbility*>& ActiveAbilities);


	/**
	* 根据标签获取技能的冷却时间
	* @param CooldownTags 需要获取冷却时间的技能标签
	* @param TimeRemaining 技能冷却的剩余时间，需要返回的
	* @param CooldownDuration 技能的冷却间隔(固定值)，需要返回的
	* @return bool 获取成功返回true，否则返回false
	*/
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		bool GetCooldownRemainingForTag(const FGameplayTagContainer& CooldownTags, float& TimeRemaining, float& CooldownDuration);

protected:
	
	/**
	* 角色类型
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UCharacterType CType;

	/**
	* 角色等级，该参数会影响能力系统对伤害的计算
	*/
	UPROPERTY(EditAnywhere, Replicated, Category = Abilities)
		int32 CharacterLevel;

	/**
	* 一些伴随着角色本身而存在的固有的能力，和外部系统无关的能力
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities)
		TArray<TSubclassOf<URPGGameplayAbility>> GameplayAbilities;
	
	/**
	* 初始时，一些默认插槽的能力，可以理解为一些初始的武器和技能
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities)
		TMap<FRPGItemSlot, TSubclassOf<URPGGameplayAbility>> DefaultSlottedAbilities;

	/**
	* 角色身上一些负面的效果
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Abilities)
		TArray<TSubclassOf<UGameplayEffect>> PassiveGameplayEffects;

	/**
	* 能力系统组件,使用它来控制能力的添加、移除和激活
	*/
	UPROPERTY()
		URPGAbilitySystemComponent* AbilitySystemComponent;

	/**
	* 角色属性，例如血量、法力值、基础伤害、移速等信息，这里我们使用URPGAttributeSet来存储角色属性，而不是单独在该类中定义
	* 原因在于角色的属性有时需要被[能力系统]修改，使用这样定义，方便[能力系统]后续的修改
	* 当然，我们也会在此类中定义一些读取相关属性的方法
	*/
	UPROPERTY()
		URPGAttributeSet* AttributeSet;

	/**
	* 背包源接口，其实就是 PlayerController
	*/
	UPROPERTY()
		TScriptInterface<IRPGInventoryInterface> InventorySource;

	/**
	* 能力是否被初始化，此变量是一个标志变量
	* 当默认的能力和背包中的能力被初始化，该值为true
	*/
	UPROPERTY()
		bool bAbilitiesInitialized;

	/**
	* 插槽中绑定的能力
	*/
	UPROPERTY()
		TMap<FRPGItemSlot, FGameplayAbilitySpecHandle> SlottedAbilities;

	/**
	* 背包变更事件句柄，绑定URPGInventoryInterface中的FOnInventoryLoadedNative事件
	*/
	FDelegateHandle InventoryUpdatedHandle;
	
	/**
	* 背包加载完成事件的句柄，绑定URPGInventoryInterface中的FOnSlotItemChangedNative会话
	*/
	FDelegateHandle InventoryLoadedHandle;

	/**
	* 插槽变更事件，用于绑定绑定URPGInventoryInterface中的FOnInventoryLoadedNative事件回调
	* @param ItemSlot 变更的插槽
	* @param Item 变更的物品
	*/
	void OnItemSlotChanged(FRPGItemSlot ItemSlot, URPGItem* Item);


	/**
	* 当发生伤害时，角色自身需要处理伤害事件，此事件需要由蓝图来实现。
	* @param DamageAmount 伤害量
	* @param HitInfo 碰撞信息
	* @param DamageTags 伤害标签
	* @param InstigatorCharacter 伤害发起角色
	* @param InstigatorActor 伤害发起的Actor，可能是武器、技能等
	*/
	UFUNCTION(BlueprintImplementableEvent)
		void OnDamaged(
			float DamageAmount,
			const FHitResult& HitInfo,
			const FGameplayTagContainer& DamageTags,
			ARPGCharacterBase* InstigatorCharacter,
			AActor* InstigatorActor
		);

	//--------------------下面是一些蓝图需要实现属性的 OnChanged 事件 start---------------------------
	/**
	* 血量改变
	* @param DeltaValue 血量的变化量
	* @param EventTags 处理血量变化的标签
	*/
	UFUNCTION(BlueprintImplementableEvent)
		void OnHealthChanged(float DeltaValue, const FGameplayTagContainer& EventTags);

	/**
	* 法力值改变
	* @param DeltaValue 法力值变更
	* @param EventTags 事件标签
	*/
	UFUNCTION(BlueprintImplementableEvent)
		void OnManaChanged(float DeltaValue, const FGameplayTagContainer& EventTags);

	/**
	* 移速改变
	*/
	UFUNCTION(BlueprintImplementableEvent)
		void OnMoveSpeedChanged(float DeltaValue, const FGameplayTagContainer& EventTags);
	//--------------------下面是一些蓝图需要实现属性的 OnChanged 事件 end---------------------------
	
	/**
	* 刷新插槽能力
	*/
	void RefreshSlottedGameplayAbilities();
	
	// 添加和移除初始的能力
	void AddStartupGameplayAbilities();
	void RemoveStartupGameplayAbilities();

	/**
	* 获取默认插槽和背包插槽中的能力规格
	* @param SlotedAbilitySpecs 需要返回的所有插槽中的能力规格
	*/
	void GetSlottedAbilitySpecs(TMap<FRPGItemSlot, FGameplayAbilitySpec>& SlottedAbilitySpecs);

	
	/**
	* 添加插槽中的能力
	*/
	void AddSlottedGameplayAbilities();

	/**
	* 移除插槽中的能力
	* @param bRemoveAll 是否全部移除
	*/
	void RemoveSlottedGameplayAbilities(bool bRemoveAll = false);

	
	//-----------------下面的方法是处理角色基本属性改变的，在URPGAttributeSet中调用----------------------------
	
	friend URPGAttributeSet;

	/**
	* 处理伤害
	* @param DamageAmout 伤害量
	* @param HitResult 碰撞结果
	* @param DamageTags 伤害所带的标签
	* @param InstigatorCharacter 伤害发起角色
	* @param InstigatorActor 伤害发起的Actor，可能是武器或者技能
	*/
	virtual void HandleDamage(
		float DamageAmount,
		const FHitResult& HitResult,
		const FGameplayTagContainer DamageTags,
		ARPGCharacterBase* InstigatorCharater,
		AActor* InstigatorActor
	);

	/**
	* 处理血量改变
	* @param DeltaValue 血量变化量
	* @param EventTags 产生此事件的标签
	*/
	virtual void HandleHealthChange(float DeltaValue, const FGameplayTagContainer& EventTags);

	/**
	* 处理法力值的改变
	* @param DeltaValue 法力值变化量
	* @param EventTags 产生此事件的标签
	*/
	virtual void HandleManaChange(float DeltaValue, const FGameplayTagContainer& EventTags);

	/**
	* 处理移动速度改变
	* @param DeltaValue 移动速度变化量
	* @param EventTags 产生此事件的标签
	*/
	virtual void HandleMoveSpeed(float DeltaValue, const FGameplayTagContainer& EventTags);
};