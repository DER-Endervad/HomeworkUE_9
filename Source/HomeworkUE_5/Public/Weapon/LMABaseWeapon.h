// LeaveMeAlone Game by Netologiya. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LMABaseWeapon.generated.h"

class USkeletalMeshComponent;
class USoundWave;
class UNiagaraSystem;

DECLARE_MULTICAST_DELEGATE(FIsBulletsEmpty);

USTRUCT(BlueprintType)
struct FAmmoWeapon
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	int32 Bullets;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	int32 Clips;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	bool Infinite;
};

UCLASS()
class HOMEWORKUE_5_API ALMABaseWeapon : public AActor
{
	GENERATED_BODY()

public:
	ALMABaseWeapon();

	void Fire();
	void ChangeClip();
	bool CanReload();
	FIsBulletsEmpty IsBulletsEmpty;
	FAmmoWeapon GetCurrentAmmoWeapon() const { return CurrentAmmoWeapon; }	

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	USkeletalMeshComponent* WeaponComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	float TraceDistance = 800.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	FAmmoWeapon AmmoWeapon{30, 0, true};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	USoundWave* ShootWave;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	UNiagaraSystem* TraceEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	FString TraceName = "Tracer";

	void SpawnTrace(const FVector& TraceStart, const FVector& TraceEnd);

	virtual void BeginPlay() override;

	void Shoot();

	void DecrementBullets();
	bool IsCurrentClipEmpty() const;

public:
	virtual void Tick(float DeltaTime) override;

private:
	FAmmoWeapon CurrentAmmoWeapon;
};
