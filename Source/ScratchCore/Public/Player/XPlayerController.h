#pragma once

#include "CoreMinimal.h"
#include "GameFramework\PlayerController.h"
#include "XPlayerController.generated.h"

/*
 * Player controller that handles input, actions, etc.
 */
UCLASS()
class SCRATCHCORE_API AXPlayerController : public APlayerController {
	GENERATED_BODY()
public:
	AXPlayerController();
protected:

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	TObjectPtr<class AXBaseCharacter> PlayerCharacterRef = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	TObjectPtr<class AXPlayerCameraManager> PlayerCameraManagerRef = nullptr;

	virtual void SetupInputComponent() override;

	virtual void OnPossess(class APawn* PawnToPossess) override;

	virtual void BeginPlay() override;

	/* Movement Functions | Grounded Movement */
	UFUNCTION()
	void AdjustGroundedOrientation();
	
	UFUNCTION()
	void ForwardMovement(float Value);

	UFUNCTION()
	void LateralMovement(float Value);

	UFUNCTION()
	void JumpAction();

	/* Trigger Functions  | Climbing */
	UFUNCTION()
	void StartClimbing();
	UFUNCTION()
	void StopClimbing();

	/* Camera Functions | Orbit Camera */
	UFUNCTION()
	void OrbitCameraYaw(float Value);

	UFUNCTION()
	void OrbitCameraPitch(float Value);
};