#include "RPGAbilityTask_PlayMontageAndWaitForEvent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "AbilitySystemGlobals.h"

URPGAbilityTask_PlayMontageAndWaitForEvent::URPGAbilityTask_PlayMontageAndWaitForEvent(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	Rate = 1.0f;
	bStopWhenAbilityEnds = true;
}

void URPGAbilityTask_PlayMontageAndWaitForEvent::Activate()
{
	if (Ability == nullptr) {
		return;
	}
	bool bPlayedMontage = false;
	URPGAbilitySystemComponent* RPGAbilitySystemComponent = GetTargetASC();
	if (RPGAbilitySystemComponent) {
		const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
		UAnimInstance* AminIntance = ActorInfo->GetAnimInstance();
		if (AminIntance) {
			EventHandle = RPGAbilitySystemComponent->AddGameplayEventTagContainerDelegate( // 绑定GameplayEvent
				EventTags,
				FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(
					this,
					&URPGAbilityTask_PlayMontageAndWaitForEvent::OnGameplayEvent
				)
			);

			if (RPGAbilitySystemComponent->PlayMontage( // 播放蒙太奇
				Ability,
				Ability->GetCurrentActivationInfo(),
				MontageToPlay,
				Rate,
				StartSection
			) > 0.0f) {
				if (ShouldBroadcastAbilityTaskDelegates() == false) { // 如果不设置通知会话，程序到这里结束，不做额外的会话绑定
					return;
				}
				
				CancelledHandle = Ability->OnGameplayAbilityCancelled.AddUObject( // 绑定Ability 取消事件
					this,
					&URPGAbilityTask_PlayMontageAndWaitForEvent::OnAbilityCancelled
				);

				BlendingOutDelegate.BindUObject(this, &URPGAbilityTask_PlayMontageAndWaitForEvent::OnMontageBlendingOut); // 绑定动画的混合事件
				AminIntance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MontageToPlay);

				MontageEndDelegate.BindUObject(this, &URPGAbilityTask_PlayMontageAndWaitForEvent::OnMontageEnded); // 绑定动画取消事件
				AminIntance->Montage_SetEndDelegate(MontageEndDelegate, MontageToPlay);
				
				ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
				if (Character &&
					(Character->GetLocalRole() == ROLE_Authority || Character->GetLocalRole() == ROLE_AutonomousProxy) &&
					(Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)
				) {
					Character->SetAnimRootMotionTranslationScale(AnimRootMotionTranslationScale);
				}
				bPlayedMontage = true;
			}
		}
		else {
			ABILITY_LOG(Error, TEXT("URPGAbilityTask_PlayMontageAndWaitForEvent Get AmniInstance Error: AnimInstance is nullper!"));
		}
	}
	else {
		ABILITY_LOG(Error, TEXT("URPGAbilityTask_PlayMontageAndWaitForEvent Get RPGAbilitySystemComponent Error: RPGAbilitySystemComponent is nullper!"));
	}
	SetWaitingOnAvatar();
}

void URPGAbilityTask_PlayMontageAndWaitForEvent::ExternalCancel()
{
	check(AbilitySystemComponent);
	OnAbilityCancelled();
	Super::ExternalCancel();
}

FString URPGAbilityTask_PlayMontageAndWaitForEvent::GetDebugString() const
{
	UAnimMontage* PlayingMontage = nullptr;
	if (Ability) {
		const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
		UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
		if (AnimInstance) {
			PlayingMontage = AnimInstance->Montage_IsActive(MontageToPlay) ? MontageToPlay : AnimInstance->GetCurrentActiveMontage();
		}
	}
	return FString::Printf(TEXT("PlayMontageAndWaitForEvent. MontageToPlay: %s, CurrentMontageToPlay: %s."), *GetNameSafe(MontageToPlay), *GetNameSafe(PlayingMontage));
}

void URPGAbilityTask_PlayMontageAndWaitForEvent::OnDestroy(bool bInOwnerFinished)
{
	if (Ability) {
		Ability->OnGameplayAbilityCancelled.Remove(CancelledHandle); // 移除[能力取消]会话绑定
	
		if (bInOwnerFinished && bStopWhenAbilityEnds) { // 停止正在播放的蒙太奇动画
			StopPlayingMontage();
		}
	}

	URPGAbilitySystemComponent* RPGAbilitySystemComponent = GetTargetASC(); // 解除GameplayEvent 会话绑定
	if (RPGAbilitySystemComponent) {
		RPGAbilitySystemComponent->RemoveGameplayEventTagContainerDelegate(EventTags, EventHandle);
	}
	Super::OnDestroy(bInOwnerFinished);
}

URPGAbilityTask_PlayMontageAndWaitForEvent* URPGAbilityTask_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(UGameplayAbility* OwningAbility, FName TaskInstanceName, UAnimMontage* MontageToPlay, FGameplayTagContainer EventTags, float Rate, FName StartSection, bool bStopWhenAbilityEnds, float AnimRootMotionTranslationScala)
{
	UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Rate(Rate);
	URPGAbilityTask_PlayMontageAndWaitForEvent* RPGAbilityTask = NewAbilityTask<URPGAbilityTask_PlayMontageAndWaitForEvent>(OwningAbility, TaskInstanceName);
	RPGAbilityTask->MontageToPlay = MontageToPlay;
	RPGAbilityTask->EventTags = EventTags;
	RPGAbilityTask->Rate = Rate;
	RPGAbilityTask->StartSection = StartSection;
	RPGAbilityTask->bStopWhenAbilityEnds = bStopWhenAbilityEnds;
	RPGAbilityTask->AnimRootMotionTranslationScale = AnimRootMotionTranslationScala;
	
	return RPGAbilityTask;
}

bool URPGAbilityTask_PlayMontageAndWaitForEvent::StopPlayingMontage()
{
	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	if (!ActorInfo) {
		return false;
	}
	UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
	if (!AnimInstance) {
		return false;
	}
	if (AbilitySystemComponent && Ability) {
		if (AbilitySystemComponent->GetAnimatingAbility() == Ability&&
			AbilitySystemComponent->GetCurrentMontage() == MontageToPlay
		) {
			FAnimMontageInstance* AnimMontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageToPlay);
			if (AnimMontageInstance) {
				AnimMontageInstance->OnMontageBlendingOutStarted.Unbind(); // 解绑会话
				AnimMontageInstance->OnMontageEnded.Unbind();
			}
			AbilitySystemComponent->CurrentMontageStop();
			return true;
		}
	}
	return false;
}

URPGAbilitySystemComponent* URPGAbilityTask_PlayMontageAndWaitForEvent::GetTargetASC()
{
	return Cast<URPGAbilitySystemComponent>(AbilitySystemComponent);
}

void URPGAbilityTask_PlayMontageAndWaitForEvent::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (Ability && Ability->GetCurrentMontage() == MontageToPlay) {
		if (Montage == MontageToPlay) {
			AbilitySystemComponent->ClearAnimatingAbility(Ability);
			
			ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
			if (Character && (Character->GetLocalRole() == ROLE_Authority ||
				(Character->GetLocalRole() == ROLE_AutonomousProxy && 
					Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted))
			) {
				Character->SetAnimRootMotionTranslationScale(AnimRootMotionTranslationScale);
			}
		}
		if (bInterrupted) {
			if (ShouldBroadcastAbilityTaskDelegates()) {
				OnInterrupted.Broadcast(FGameplayTag(), FGameplayEventData());
			}
		}
		else {
			if (ShouldBroadcastAbilityTaskDelegates()) {
				OnBlendOut.Broadcast(FGameplayTag(), FGameplayEventData());
			}
		}
	}
}

void URPGAbilityTask_PlayMontageAndWaitForEvent::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted) {
		if (ShouldBroadcastAbilityTaskDelegates()) {
			OnComplete.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}
	EndTask();
}


void URPGAbilityTask_PlayMontageAndWaitForEvent::OnAbilityCancelled()
{
	if (StopPlayingMontage()) {
		if (ShouldBroadcastAbilityTaskDelegates()) {
			OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
		}
	}
}

void URPGAbilityTask_PlayMontageAndWaitForEvent::OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload)
{
	if (ShouldBroadcastAbilityTaskDelegates()) {
		FGameplayEventData TempData = *Payload;
		TempData.EventTag = EventTag;
		EventReceived.Broadcast(EventTag, TempData);
	}
}
