// LeaveMeAlone Game by Netologiya. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LMADefaultCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ULMAHealthComponent;
class UAnimMontage;
class ALMABaseWeapon;
class ULMAWeaponComponent;

UCLASS()
class HOMEWORKUE_5_API ALMADefaultCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ALMADefaultCharacter();

	UFUNCTION()
	ULMAHealthComponent* GetHealthComponent() const { return HealthComponent; }  
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components|Health")
	ULMAHealthComponent* HealthComponent;

	UPROPERTY()
	UDecalComponent* CurrentCursor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
	UMaterialInterface* CursorMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
	FVector CursorSize = FVector(20.0f, 40.0f, 40.0f);

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* DeathMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components|Weapon")
	ALMABaseWeapon* BaseWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	ULMAWeaponComponent* WeaponComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sprint")
	float Stamina = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sprint")
	float MaxStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprint")
	float StaminaTime = 0.5f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Sprint")
	bool Shift = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Walk")
	bool BackForward = false;
	//bool BaF = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Walk")
	bool RightLeft = false;
	//bool RaL = false;

	UPROPERTY()
	UWorld* world;

	float ShiftTimeOld = 0.0f;
	float ShiftTimeNow = 0.0f;
	/*float STimeOld = 0.0f;
	float STimeNow = 0.0f;*/

	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	float YRotation = -75.0f;

	float ArmLength = 1400.0f;
	float ArmMin = 500.0f;
	float ArmMax = 1500.0f;

	float FOV = 55.0f;

	float SpeedWalk = 300.0f;
	float SpeedSprint = 600.0f;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void MoveCamera(float Value);

	void Sprint();
	void SprintStop();
	//void Sprint(float Value);

	void OnDeath();
	// void OnHealthChanged(float NewHealth);

	void RotationPlayerOnCursor();
};
