// LeaveMeAlone Game by Netologiya. All Rights Reserved.

#include "Player/LMADefaultCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/LMAHealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/Engine.h"
#include "Weapon/LMABaseWeapon.h"
#include "Components/LMAWeaponComponent.h"

ALMADefaultCharacter::ALMADefaultCharacter() {

	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->SetUsingAbsoluteRotation(true);
	SpringArmComponent->TargetArmLength = ArmLength;
	SpringArmComponent->SetRelativeRotation(FRotator(YRotation, 0.0f, 0.0f));
	SpringArmComponent->bDoCollisionTest = false;
	SpringArmComponent->bEnableCameraLag = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->SetFieldOfView(FOV);
	CameraComponent->bUsePawnControlRotation = false;

	HealthComponent = CreateDefaultSubobject<ULMAHealthComponent>("HealthComponent");

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	WeaponComponent = CreateDefaultSubobject<ULMAWeaponComponent>("Weapon");
}

void ALMADefaultCharacter::BeginPlay() {
	Super::BeginPlay();

	if (CursorMaterial) {
		CurrentCursor = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), CursorMaterial, CursorSize, FVector(0));
	}

	//OnHealthChanged(HealthComponent->GetHealth());
	HealthComponent->OnDeath.AddUObject(this, &ALMADefaultCharacter::OnDeath);
	//HealthComponent->OnHealthChanged.AddUObject(this, &ALMADefaultCharacter::OnHealthChanged);
	world = GetWorld();
}

void ALMADefaultCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (!(HealthComponent->IsDead())) {
		RotationPlayerOnCursor();
	}

	if (IsValid(world) && Stamina < MaxStamina && ((!Shift || (!BackForward && !RightLeft)) || Stamina <= 0)) {
		ShiftTimeNow = world->GetTimeSeconds();
		if ((ShiftTimeNow - ShiftTimeOld) >= StaminaTime) {
			if (RightLeft || BackForward) {
				Stamina = Stamina + 1;
			} else {
				Stamina = Stamina + 2;
			}
			ShiftTimeOld = ShiftTimeNow;
			if (Stamina > MaxStamina) {
				Stamina = MaxStamina;
			}
			//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Stamina = %f"), Stamina));
		}
	}

	if (Shift && Stamina > 0 && (BackForward || RightLeft)) {
		Shift = true;
		Stamina = Stamina - 0.25;
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Stamina = %f"), Stamina));
	} else {
		SprintStop();
	}

	/*APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		FHitResult ResultHit;
		PC->GetHitResultUnderCursor(ECC_GameTraceChannel1, true, ResultHit);
		float FindRotatorResultYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;
		SetActorRotation(FQuat(FRotator(0.0f, FindRotatorResultYaw, 0.0f)));
		if (CurrentCursor)
		{
			CurrentCursor->SetWorldLocation(ResultHit.Location);
		}
	}*/
}

void ALMADefaultCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ALMADefaultCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ALMADefaultCharacter::MoveRight);
	PlayerInputComponent->BindAxis("MoveCamera", this, &ALMADefaultCharacter::MoveCamera);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ALMADefaultCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ALMADefaultCharacter::SprintStop);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, WeaponComponent, &ULMAWeaponComponent::Fire);
	PlayerInputComponent->BindAction("Fire", IE_Released, WeaponComponent, &ULMAWeaponComponent::FireStop);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, WeaponComponent, &ULMAWeaponComponent::ThisReload);
	// PlayerInputComponent->BindAxis("Sprint", this, &ALMADefaultCharacter::Sprint); Старый бег
	// PlayerInputComponent->BindAction("Shoot", IE_Pressed, BaseWeapon, &ALMABaseWeapon::Fire); было сказанно в вебинаре.
	// PlayerInputComponent->BindAxis("Fire", WeaponComponent, &ULMAWeaponComponent::Fire);
}

void ALMADefaultCharacter::MoveForward(float Value) {
	AddMovementInput(GetActorForwardVector(), Value);
	if (Value != 0) {
		BackForward = true;
	} else {
		BackForward = false;
	}
}

void ALMADefaultCharacter::MoveRight(float Value) {
	AddMovementInput(GetActorRightVector(), Value);
	if (Value != 0) {
		RightLeft = true;
	} else {
		RightLeft = false;
	}
}

void ALMADefaultCharacter::MoveCamera(float Value) {
	ArmLength = ArmLength + (Value*2);
	if (ArmLength > ArmMax) {
		ArmLength = ArmMax;
	} else if (ArmLength < ArmMin) {
		ArmLength = ArmMin;
	}
	SpringArmComponent->TargetArmLength = ArmLength;
}

void ALMADefaultCharacter::Sprint() {
	GetCharacterMovement()->MaxWalkSpeed = SpeedSprint;
	Shift = true;
}

void ALMADefaultCharacter::SprintStop() {
	GetCharacterMovement()->MaxWalkSpeed = SpeedWalk;
	Shift = false;
}

// void ALMADefaultCharacter::Sprint(float Value) {
//	world = GetWorld();
//	if (IsValid(world) && Stamina < 100 && ((Value <= 0 || (!BaF && !RaL)) || Stamina <= 0)) {
//		STimeNow = world->GetTimeSeconds();
//		if ((STimeNow - STimeOld) >= StaminaTime) {
//			if (RaL || BaF) {
//				Stamina = Stamina + 0.5;
//			} else {
//				Stamina = Stamina + 1;
//			}
//			STimeOld = STimeNow;
//			if (Stamina > 100) {
//				Stamina = 100;
//			}
//			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Stamina = %f"), Stamina));
//		}
//	}
//
//	if (Stamina > 0 && Value > 0 && (BaF || RaL)) {
//		Shift = true;
//		GetCharacterMovement()->MaxWalkSpeed = SpeedSprint;
//		Stamina = Stamina - (Value / 4);
//		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Stamina = %f"), Stamina));
//	}
//
//	if (Value <= 0 || Stamina <= 0 || (!BaF && !RaL)) {
//		Shift = false;
//		GetCharacterMovement()->MaxWalkSpeed = SpeedWalk;
//	}
// }

void ALMADefaultCharacter::OnDeath() {
	CurrentCursor->DestroyRenderState_Concurrent();

	PlayAnimMontage(DeathMontage);

	GetCharacterMovement()->DisableMovement();

	SetLifeSpan(5.0f);

	if (Controller) {
		Controller->ChangeState(NAME_Spectating);
	}
	WeaponComponent->DestroyWeapon();
}

//void ALMADefaultCharacter::OnHealthChanged(float NewHealth) {
//	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Health = %f"), NewHealth));
//}

void ALMADefaultCharacter::RotationPlayerOnCursor() {
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC) {
		FHitResult ResultHit;
		PC->GetHitResultUnderCursor(ECC_GameTraceChannel1, true, ResultHit);
		float FindRotatorResultYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;
		SetActorRotation(FQuat(FRotator(0.0f, FindRotatorResultYaw, 0.0f)));
		if (CurrentCursor) {
			CurrentCursor->SetWorldLocation(ResultHit.Location);
		}
	}
}
