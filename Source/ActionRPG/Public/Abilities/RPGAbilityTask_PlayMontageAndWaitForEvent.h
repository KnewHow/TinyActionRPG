#pragma once

#include "ActionRPG.h"
#include "Tasks/AbilityTask.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "RPGAbilitySystemComponent.h"
#include "RPGAbilityTask_PlayMontageAndWaitForEvent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRPGPlayMontageAndWaitForEventDelegate, FGameplayTag, EventTag, FGameplayEventData, EventData);
/**
* 这是一个播放蒙太奇动画并等待其结束、取消等操作的任务。
* 关于如果去写一个 AbilityTask, 可以参考 UAbilityTask的注释内容，里面说到需要去实现 OnDestory和Active,已经定义一个静态的方法
* 还可以去参考UAbilityTask_WaitOverlap和UAbilityTask_WaitTargetData来做更加细致的分析
* 
*/
UCLASS()
class ACTIONRPG_API URPGAbilityTask_PlayMontageAndWaitForEvent: public UAbilityTask {
	GENERATED_BODY()
public:
	URPGAbilityTask_PlayMontageAndWaitForEvent(const FObjectInitializer& ObjectInitializer);
	
	/**
	* 根据文档必须要实现的函数用来开始任务，这是任务真正开始执行的方法，而不是在静态的工厂方法中
	* 在此函数中，我们将会对任务做一些事件的绑定和播放蒙太奇动画
	*/
	virtual void Activate() override;

	/**
	* 当任务被外部取消的时候，调用此方法。
	* 再次函数中，我们会调用 OnAbilityCancelled 来广播取消事件
	*/
	virtual void ExternalCancel() override;

	/**
	* 获取描述此任务的调试字符串
	*/
	virtual FString GetDebugString() const override;

	/**
	* 当任务结束时或者任务拥有者结束时，调用此方法
	* @param bInOwnerFinished 能力是自己结束
	*/
	virtual void OnDestroy(bool bInOwnerFinished) override;

	/**
	* 一个动画播放完成会话，在蓝图中，他就是一个回调的引脚
	*/
	UPROPERTY(BlueprintAssignable)
		FRPGPlayMontageAndWaitForEventDelegate OnComplete;

	/**
	* 蒙太奇动画混合会话，在蓝图中，他就是一个回调的引脚
	*/
	UPROPERTY(BlueprintAssignable)
		FRPGPlayMontageAndWaitForEventDelegate OnBlendOut;

	/**
	* 蒙太奇动画中断会话，在蓝图中，他就是一个回调的引脚
	*/
	UPROPERTY(BlueprintAssignable)
		FRPGPlayMontageAndWaitForEventDelegate OnInterrupted;
	/**
	* 能力取消会话，在蓝图中，他就是一个回调的引脚
	*/
	UPROPERTY(BlueprintAssignable)
		FRPGPlayMontageAndWaitForEventDelegate OnCancelled;
	
	/**
	* GameplayEvent 成功接收时触发的会话，在蓝图中，他就是一个回调的引脚
	*/
	UPROPERTY(BlueprintAssignable)
		FRPGPlayMontageAndWaitForEventDelegate EventReceived;


	/**
	* 一个工厂函数创建 URPGAbilityTask_PlayMontageAndWaitForEvent
	* 去播放蒙太奇动画并且等待他结束。
	* 如果接收到一个GameplayEvent,里面的 EventReceived 会话将会被激活
	* @param OwningAbility 需要执行的能力
	* @param TaskInstanceName 任务名称
	* @param MontageToPlay 将要播放的蒙太奇动画
	* @param EventTags 事件标签组
	* @param Rate 动画播放速率
	* @param StartSection 蒙太奇动画开始播放的片段
	* @param bStopWhenAbilityEnds 当能力停止时，是否停止播放动画
	* @param AnimRootMotionTranslationScala 根骨骼动画移动范围
	* @return 返回该任务的实例指针
	*/
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly="TRUE"))
	static URPGAbilityTask_PlayMontageAndWaitForEvent* PlayMontageAndWaitForEvent(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName,
		UAnimMontage* MontageToPlay,
		FGameplayTagContainer EventTags,
		float Rate = 1.0f,
		FName StartSection = NAME_None,
		bool bStopWhenAbilityEnds = true,
		float AnimRootMotionTranslationScala = 1.0f
	);

private:
	/**
	* 关联的蒙太奇动画
	*/
	UPROPERTY()
		UAnimMontage* MontageToPlay;

	/**
	* 事件标签，用于OnGameplayEvent会话的绑定
	*/
	UPROPERTY()
		FGameplayTagContainer EventTags;

	/**
	* 蒙太奇动画播放速率，例如1倍速和1.5倍速
	*/
	UPROPERTY()
		float Rate;

	/**
	* 蒙太奇开始播放的片段
	*/
	UPROPERTY()
		FName StartSection;

	/**
	* 角色根骨骼动画移动范围
	*/
	UPROPERTY()
		float AnimRootMotionTranslationScale;

	/**
	* 当能力结束时，是否停止播放蒙太奇动画
	*/
	UPROPERTY()
		bool bStopWhenAbilityEnds;

	/**
	* 停止正在播放的蒙太奇动画
	* @return 停止成功返回true,否则返回false
	*/
	bool StopPlayingMontage();

	/**
	* 获取目标的能力系统组件
	*/
	URPGAbilitySystemComponent* GetTargetASC();

	/**
	* 被蒙太奇动画实例(AnimInstace)绑定的蒙太奇混合事件
	* @param Montage 播放的蒙太奇动画
	* @param bInterrupted 是否广播中断会话
	*/
	void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);

	/**
	* 被蒙太奇动画实例(AnimInstace)绑定的Ended事件
	*/
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	/**
	* 被GameplayAbility绑定取消事件
	*/
	void OnAbilityCancelled();
	
	/**
	* 被RPGAbilitySystemComponent绑定的Gameplay事件，也是成功完成一个GameplayAbility后被回调的事件
	* 回调此事件后将会进行广播，来处理GameplayAbility完成后的一些操作，例如伤害的应用等。
	*/
	void OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload);

	/**
	* 一个蒙太奇混合的本地会话，用于和[OnMontageBlendingOut] 做绑定，并且传递给动画实例(AnimInstance)
	*/
	FOnMontageBlendingOutStarted BlendingOutDelegate;

	/**
	* 一个蒙太奇终止的本地会话，用于和[OnMontageEnded] 做绑定，并且传递给动画实例(AnimInstance)
	*/
	FOnMontageEnded MontageEndDelegate;

	/**
	* Ability 取消会话的句柄，保留绑定[OnAbilityCancelled]的结果，本地保存一份用于OnDestory时解除会话绑定
	*/
	FDelegateHandle CancelledHandle;

	/**
	* AbilitySystemComponent 绑定 OnGameplayEvent 的句柄，本地存储一份为了 OnDestory时进行解除绑定
	*/
	FDelegateHandle EventHandle;

};