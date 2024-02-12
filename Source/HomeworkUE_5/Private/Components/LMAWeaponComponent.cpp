// LeaveMeAlone Game by Netologiya. All Rights Reserved.

#include "Components/LMAWeaponComponent.h"
#include "Animations/LMAReloadFinishedAnimNotify.h"
#include "GameFramework/Character.h"
#include "Weapon/LMABaseWeapon.h"

ULMAWeaponComponent::ULMAWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void ULMAWeaponComponent::Fire() {
	IsFire = true;
}

void ULMAWeaponComponent::FireStop() {
	IsFire = false;
	FireAnimation = false;
}

void ULMAWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	SpawnWeapon();
	InitAnimNotify();
	world = GetWorld();
}

bool ULMAWeaponComponent::GetCurrentWeaponAmmo(FAmmoWeapon& AmmoWeapon) const {
	if (Weapon) {
		AmmoWeapon = Weapon->GetCurrentAmmoWeapon();
		return true;
	}
	return false;
}

void ULMAWeaponComponent::ThisReload() {
	if (!CanReload()) return;
	Weapon->ChangeClip();
	AnimReloading = true;
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	Character->PlayAnimMontage(ReloadMontage);
}

void ULMAWeaponComponent::DestroyWeapon() {
	Weapon->K2_DestroyActor();
}

void ULMAWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsValid(Weapon) && !AnimReloading && IsFire) {
		FireTimeNow = world->GetTimeSeconds();
		FireAnimation = true;
		if ((FireTimeNow - FireTimeOld) >= FireTime) {
			Weapon->Fire();
			FireTimeOld = FireTimeNow;
		}
	}
	else {
		FireAnimation = false;
	}
}

void ULMAWeaponComponent::SpawnWeapon()
{
	Weapon = GetWorld()->SpawnActor<ALMABaseWeapon>(WeaponClass);
	if (Weapon)
	{
		const auto Character = Cast<ACharacter>(GetOwner());
		if (Character)
		{
			FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
			Weapon->AttachToComponent(Character->GetMesh(), AttachmentRules, "r_Weapon_Socket"); // WeaponSocket
		}
	}
	Weapon->IsBulletsEmpty.AddUObject(this, &ULMAWeaponComponent::ThisReload);
}

void ULMAWeaponComponent::InitAnimNotify()
{
	if (!ReloadMontage) return;

	const auto NotifiesEvents = ReloadMontage->Notifies;
	for (auto NotifyEvent : NotifiesEvents)
	{
		auto ReloadFinish = Cast<ULMAReloadFinishedAnimNotify>(NotifyEvent.Notify);
		if (ReloadFinish)
		{
			ReloadFinish->OnNotifyReloadFinished.AddUObject(this, &ULMAWeaponComponent::OnNotifyReloadFinished);
			break;
		}
	}
}

void ULMAWeaponComponent::OnNotifyReloadFinished(USkeletalMeshComponent* SkeletalMesh)
{
	const auto Character = Cast<ACharacter>(GetOwner());
	if (Character->GetMesh() == SkeletalMesh)
	{
		AnimReloading = false;
	}
}

bool ULMAWeaponComponent::CanReload() const
{
	return !AnimReloading && Weapon->CanReload();
}

void ULMAWeaponComponent::Reload() { ThisReload(); }

