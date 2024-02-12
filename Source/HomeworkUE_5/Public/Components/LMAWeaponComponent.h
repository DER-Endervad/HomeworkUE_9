// LeaveMeAlone Game by Netologiya. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LMAWeaponComponent.generated.h"

class ALMABaseWeapon;
class UAnimMontage;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class HOMEWORKUE_5_API ULMAWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULMAWeaponComponent();

	void Fire();
	void FireStop();
	void Reload();
	void ThisReload();
	void DestroyWeapon();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<ALMABaseWeapon> WeaponClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	UAnimMontage* ReloadMontage;

	UPROPERTY()
	UWorld* world;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire")
	float FireTime = 0.25f;

	float FireTimeOld = 0.0f;
	float FireTimeNow = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Fire")
	bool IsFire = false;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	bool GetCurrentWeaponAmmo(FAmmoWeapon& AmmoWeapon) const;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Fire")
	bool FireAnimation = false;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
	ALMABaseWeapon* Weapon = nullptr;

	bool AnimReloading = false;

	void SpawnWeapon();
	void InitAnimNotify();

	void OnNotifyReloadFinished(USkeletalMeshComponent* SkeletalMesh);
	bool CanReload() const;
};
