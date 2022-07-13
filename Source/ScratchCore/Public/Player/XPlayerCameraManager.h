#pragma once

#include "CoreMinimal.h"
#include "Camera\PlayerCameraManager.h"
#include "XPlayerCameraManager.generated.h"

UCLASS()
class SCRATCHCORE_API AXPlayerCameraManager : public APlayerCameraManager {
	GENERATED_BODY()
private:
	virtual void ProcessViewRotation(float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot) override;

public:
	AXPlayerCameraManager(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player")
	TObjectPtr<class AXBaseCharacter> PlayerCharacterRef = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Player")
	void OnPossess(class AXBaseCharacter* PossessedCharacter);

	/* Player Camera Manager | Orbit Camera */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Manager | Orbit Camera")
	bool bInvertCameraYaw;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Manager | Orbit Camera")
	bool bInvertCameraPitch;

	UFUNCTION()
	void OrbitCameraYaw(float Value);

	UFUNCTION()
	void OrbitCameraPitch(float Value);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit Camera")
	float CameraOrbitSpeed;
};