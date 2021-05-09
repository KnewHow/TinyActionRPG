#include "RPGCharacterBase.h"
#include "Item/RPGItem.h"

ARPGCharacterBase::ARPGCharacterBase() {
	this->AbilitySystemComponent = CreateDefaultSubobject<URPGAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	this->AbilitySystemComponent->SetIsReplicated(true);

	this->AttributeSet = CreateDefaultSubobject<URPGAttributeSet>(TEXT("AttributeSet"));

	this->CharacterLevel = 1;
	this->bAbilitiesInitialized = false;
}

void ARPGCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	this->InventorySource = NewController;
	if (InventorySource) { // 绑定背包变更和已加载的会话
		this->InventoryUpdatedHandle = this->InventorySource->GetSlotItemChangedDelegate()
			.AddUObject(this, &ARPGCharacterBase::OnItemSlotChanged);

		this->InventoryLoadedHandle = this->InventorySource->GetInventoryLoadedDelegate()
			.AddUObject(this, &ARPGCharacterBase::RefreshSlottedGameplayAbilities);
	}

	if (this->AbilitySystemComponent) {
		this->AbilitySystemComponent->InitAbilityActorInfo(this, this);
		this->AddStartupGameplayAbilities();
	}
}

void ARPGCharacterBase::UnPossessed() {
	if (this->InventorySource) {
		if (this->InventoryUpdatedHandle.IsValid()) {
			this->InventorySource->GetSlotItemChangedDelegate().Remove(this->InventoryUpdatedHandle);
			this->InventoryUpdatedHandle.Reset();
		}

		if (this->InventoryLoadedHandle.IsValid()) {
			this->InventorySource->GetInventoryLoadedDelegate().Remove(this->InventoryLoadedHandle);
			this->InventoryLoadedHandle.Reset();
		}
	}
	this->RemoveStartupGameplayAbilities();
	this->InventorySource = nullptr;
}

void ARPGCharacterBase::OnRep_Controller()
{
	Super::OnRep_Controller();
	
	if (this->AbilitySystemComponent) {
		this->AbilitySystemComponent->RefreshAbilityActorInfo();
	}
}

UAbilitySystemComponent* ARPGCharacterBase::GetAbilitySystemComponent() const
{
	return this->AbilitySystemComponent;
}

void ARPGCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARPGCharacterBase, CharacterLevel);
}

float ARPGCharacterBase::GetHealth() const
{
	if (this->AttributeSet) {
		return this->AttributeSet->GetHealth();
	}
	return 0.0f;
}

float ARPGCharacterBase::GetMaxHealth() const
{
	if (this->AttributeSet) {
		return this->AttributeSet->GetMaxHealth();
	}
	return 0.0f;
}

float ARPGCharacterBase::GetMana() const
{
	if (this->AttributeSet) {
		return this->AttributeSet->GetMana();
	}
	return 0.0f;
}

float ARPGCharacterBase::GetMaxMana() const
{
	if (this->AttributeSet) {
		return this->AttributeSet->GetMaxMana();
	}
	return 0.0f;
}

float ARPGCharacterBase::GetDamage() const
{
	if (this->AttributeSet) {
		return this->AttributeSet->GetDamage();
	}
	return 0.0f;
}

float ARPGCharacterBase::GetMoveSpeed() const
{
	if (this->AttributeSet) {
		return this->AttributeSet->GetMoveSpeed();
	}
	return 0.0f;
}

int32 ARPGCharacterBase::GetCharacterLevel() const {
	return this->CharacterLevel;
}

bool ARPGCharacterBase::SetCharacterLevel(int32 NewLevel)
{
	if (NewLevel != this->GetCharacterLevel() && NewLevel > 0) {
		this->RemoveStartupGameplayAbilities();
		this->CharacterLevel = NewLevel;
		this->AddStartupGameplayAbilities();
		return true;
	}
	return false;
}

UCharacterType ARPGCharacterBase::GetCharacterType() const
{
	return this->CType;
}

bool ARPGCharacterBase::ActivateAbilitiesWithItemSlot(const FRPGItemSlot& ItemSlot, bool bAllowRemoteActivation)
{
	FGameplayAbilitySpecHandle* FoundHandle = this->SlottedAbilities.Find(ItemSlot);
	if (FoundHandle && this->AbilitySystemComponent) {
		return this->AbilitySystemComponent->TryActivateAbility(*FoundHandle, bAllowRemoteActivation);
	}
	return false;
}

void ARPGCharacterBase::GetActiveAbilitiesWithItemSlot(const FRPGItemSlot& ItemSlot, TArray<URPGGameplayAbility*>& ActiveAbilities)
{
	FGameplayAbilitySpecHandle* FoundHandle = this->SlottedAbilities.Find(ItemSlot);
	if (FoundHandle && this->AbilitySystemComponent) {
		FGameplayAbilitySpec* FoundSpec = this->AbilitySystemComponent->FindAbilitySpecFromHandle(*FoundHandle);
		if (FoundSpec) {
			TArray<UGameplayAbility*> Abilities =  FoundSpec->GetAbilityInstances();
			for (auto Ability : Abilities) {
				ActiveAbilities.Add(Cast<URPGGameplayAbility>(Ability));
			}
		}
	}
}

bool ARPGCharacterBase::ActivateAbilitiesWithTags(const FGameplayTagContainer& AbilityTags, bool bAllowRemoteActivation)
{
	if (this->AbilitySystemComponent) {
		return this->AbilitySystemComponent->TryActivateAbilitiesByTag(AbilityTags, bAllowRemoteActivation);
	}
	return false;
}

void ARPGCharacterBase::GetActiveAbilitiesWithTags(const FGameplayTagContainer& AbilityTags, TArray<URPGGameplayAbility*>& ActiveAbilities)
{
	if (this->AbilitySystemComponent) {
		this->AbilitySystemComponent->GetActiveAbilitiesWithTags(AbilityTags, ActiveAbilities);
	}
}

bool ARPGCharacterBase::GetCooldownRemainingForTag(const FGameplayTagContainer& CooldownTags, float& TimeRemaining, float& CooldownDuration)
{
	if (this->AbilitySystemComponent && CooldownTags.Num() > 0) {
		CooldownDuration = 0.0f;
		TimeRemaining = 0.0f;
		FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTags);
		TArray<TPair<float, float>> DurationAndTimeRemainingArray = this->AbilitySystemComponent->GetActiveEffectsTimeRemainingAndDuration(Query);
		if (DurationAndTimeRemainingArray.Num() > 0) {// find the longest time remain
			int LongestIndex = 0;
			float LongestTimeRemain = DurationAndTimeRemainingArray[0].Key;
			for (int i = 1; i < DurationAndTimeRemainingArray.Num(); ++i) {
				if (DurationAndTimeRemainingArray[i].Key > LongestTimeRemain) {
					LongestTimeRemain = DurationAndTimeRemainingArray[i].Key;
					LongestIndex = i;

				}
			}
			CooldownDuration = DurationAndTimeRemainingArray[LongestIndex].Value;
			TimeRemaining = DurationAndTimeRemainingArray[LongestIndex].Key;
		}
	}
	return false;
}



void ARPGCharacterBase::RefreshSlottedGameplayAbilities()
{
	if (this->bAbilitiesInitialized) {
		this->RemoveSlottedGameplayAbilities(false);
		this->AddSlottedGameplayAbilities();
	}
}

void ARPGCharacterBase::AddStartupGameplayAbilities()
{
	check(this->AbilitySystemComponent);
	if (GetLocalRole() == ROLE_Authority && !this->bAbilitiesInitialized) {
		for (const TSubclassOf<URPGGameplayAbility>& StartupAbility : this->GameplayAbilities) {
			this->AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartupAbility, this->GetCharacterLevel(),INDEX_NONE, this));
		}

		for (const TSubclassOf<UGameplayEffect>& PassiveGE : this->PassiveGameplayEffects) {
			FGameplayEffectContextHandle GEContextHandle = this->AbilitySystemComponent->MakeEffectContext();
			GEContextHandle.AddSourceObject(this);

			FGameplayEffectSpecHandle NewHandle = this->AbilitySystemComponent->MakeOutgoingSpec(PassiveGE, this->GetCharacterLevel(), GEContextHandle);

			if (NewHandle.IsValid()) {
				FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), this->AbilitySystemComponent);
			}
		}
		this->AddSlottedGameplayAbilities();
		this->bAbilitiesInitialized = true;

	}

}

void ARPGCharacterBase::RemoveStartupGameplayAbilities()
{
	check(this->AbilitySystemComponent);
	if (this->GetLocalRole() == ROLE_Authority && this->bAbilitiesInitialized) {
		TArray<FGameplayAbilitySpecHandle> AbilitiesToRemoved; // Clear self GameAbilities
		for (const FGameplayAbilitySpec& Spec : this->AbilitySystemComponent->GetActivatableAbilities()) {
			if (Spec.SourceObject == this && this->GameplayAbilities.Contains(Spec.Ability->GetClass())) {
				AbilitiesToRemoved.Add(Spec.Handle);
			}
		}
		for (const auto Handle : AbilitiesToRemoved) {
			this->AbilitySystemComponent->ClearAbility(Handle);
		}

		FGameplayEffectQuery Query; // remove GEs
		Query.EffectSource = this;
		this->AbilitySystemComponent->RemoveActiveEffects(Query);

		this->RemoveSlottedGameplayAbilities(true);
		this->bAbilitiesInitialized = false;
	}
}

void ARPGCharacterBase::GetSlottedAbilitySpecs(TMap<FRPGItemSlot, FGameplayAbilitySpec>& SlottedAbilitySpecs)
{
	for (const TPair<FRPGItemSlot, TSubclassOf<URPGGameplayAbility>>& Pair : this->DefaultSlottedAbilities) {
		if (Pair.Value.Get()) {
			SlottedAbilitySpecs.Add(Pair.Key, FGameplayAbilitySpec(Pair.Value, this->GetCharacterLevel(),INDEX_NONE, this));
		}
	}

	if (this->InventorySource) {
		const TMap<FRPGItemSlot, URPGItem*> SlottedItemMap = this->InventorySource->GetSlottedItemMap();
		for (const TPair<FRPGItemSlot, URPGItem*>& Pair : SlottedItemMap) {
			URPGItem* SlottedItem = Pair.Value;
			int32 AbilityLevel = this->GetCharacterLevel();
			if (SlottedItem && SlottedItem->ItemType == URPGAssetManager::WeaponItemType) {
				AbilityLevel = SlottedItem->AbilityLevel;
			}
			if (SlottedItem && SlottedItem->GrantedAbility.Get()) {
				SlottedAbilitySpecs.Add(Pair.Key, FGameplayAbilitySpec(SlottedItem->GrantedAbility, AbilityLevel, INDEX_NONE, SlottedItem));
			}
		}
	}
}

void ARPGCharacterBase::AddSlottedGameplayAbilities()
{
	TMap<FRPGItemSlot, FGameplayAbilitySpec> SlottedAbilitySpecs;
	GetSlottedAbilitySpecs(SlottedAbilitySpecs);

	for (const TPair<FRPGItemSlot, FGameplayAbilitySpec>& Pair : SlottedAbilitySpecs) {
		FGameplayAbilitySpecHandle& SpecHandle =  this->SlottedAbilities.FindOrAdd(Pair.Key);
		if (!SpecHandle.IsValid()) {
			SpecHandle = this->AbilitySystemComponent->GiveAbility(Pair.Value);
		}
	}
}

void ARPGCharacterBase::RemoveSlottedGameplayAbilities(bool bRemoveAll)
{
	TMap<FRPGItemSlot, FGameplayAbilitySpec> SlottedAbilitySpecs;
	if (!bRemoveAll) {
		GetSlottedAbilitySpecs(SlottedAbilitySpecs);
	}
	
	for (TPair<FRPGItemSlot, FGameplayAbilitySpecHandle>& Pair : this->SlottedAbilities) {
		FGameplayAbilitySpec* FoundSpec = this->AbilitySystemComponent->FindAbilitySpecFromHandle(Pair.Value);
		bool bShouldRemove = bRemoveAll || !FoundSpec;
		if (!bShouldRemove) {
			FGameplayAbilitySpec* DesiredSpec = SlottedAbilitySpecs.Find(Pair.Key);
			if (!DesiredSpec || DesiredSpec->Ability != FoundSpec->Ability || DesiredSpec->SourceObject != FoundSpec->SourceObject) {
				bShouldRemove = true;
			}
		}

		if (bShouldRemove) {
			if (FoundSpec) {
				this->AbilitySystemComponent->ClearAbility(Pair.Value);
			}
			Pair.Value = FGameplayAbilitySpecHandle(); // Rest Value to Empty
		}
	}
}

void ARPGCharacterBase::HandleDamage(
	float DamageAmount,
	const FHitResult& HitResult,
	const FGameplayTagContainer DamageTags,
	ARPGCharacterBase* InstigatorCharater,
	AActor* InstigatorActor
) {
	if (this->bAbilitiesInitialized) {
		OnDamaged(DamageAmount, HitResult, DamageTags, InstigatorCharater, InstigatorActor);
	}
}

void ARPGCharacterBase::HandleHealthChange(float DeltaValue, const FGameplayTagContainer& EventTags)
{
	if (this->bAbilitiesInitialized) {
		OnHealthChanged(DeltaValue, EventTags);
	}
}

void ARPGCharacterBase::HandleManaChange(float DeltaValue, const FGameplayTagContainer& EventTags)
{
	if (this->bAbilitiesInitialized) {
		OnManaChanged(DeltaValue, EventTags);
	}
}

void ARPGCharacterBase::HandleMoveSpeed(float DeltaValue, const FGameplayTagContainer& EventTags)
{
	this->GetCharacterMovement()->MaxWalkSpeed = this->GetMoveSpeed();
	if (this->bAbilitiesInitialized) {
		this->OnMoveSpeedChanged(DeltaValue, EventTags);
	}
}

void ARPGCharacterBase::OnItemSlotChanged(FRPGItemSlot ItemSlot, URPGItem* Item)
{
	this->RefreshSlottedGameplayAbilities();
}
