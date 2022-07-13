#include "Player/XPlayerCameraManager.h"
#include "Player/XBaseCharacter.h"
#include "Player/XPlayerCamera.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SphereComponent.h"

AXPlayerCameraManager::AXPlayerCameraManager(const FObjectInitializer& ObjectInitializer) {
	/* Component Defaults */
	CameraOrbitSpeed = 75.f;
	bInvertCameraYaw = false;
	bInvertCameraPitch = true;
}

void AXPlayerCameraManager::ProcessViewRotation(float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot)
{
	Super::ProcessViewRotation(DeltaTime, OutViewRotation, OutDeltaRot);

	//LimitViewPitch(OutViewRotation, 0.0f, 65.0f);
}

void AXPlayerCameraManager::OnPossess(class AXBaseCharacter* PossesedCharacter) {
	// Set reference to possessed character pawn
	check(PossesedCharacter);
	PlayerCharacterRef = PossesedCharacter;
	
	// Get player camera and set view target
	TObjectPtr<UXPlayerCamera> TPCamera = PlayerCharacterRef->TPCamera;
	if (PlayerCharacterRef->TPCamera != nullptr) {
		SetActorLocation(TPCamera->GetComponentLocation());
		SetViewTarget(PlayerCharacterRef);
	}
}

void AXPlayerCameraManager::OrbitCameraYaw(float Value)
{
	if (bInvertCameraYaw) Value = -Value;

	PlayerCharacterRef->AddControllerYawInput(Value * CameraOrbitSpeed * GetWorld()->GetDeltaSeconds());
}

void AXPlayerCameraManager::OrbitCameraPitch(float Value)
{
	if (bInvertCameraPitch) Value = -Value;

	PlayerCharacterRef->AddControllerPitchInput(Value * CameraOrbitSpeed * GetWorld()->GetDeltaSeconds());
}
