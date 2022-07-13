#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "XTestCharacter.generated.h"

UCLASS()
class SCRATCHCORE_API AXTestCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AXTestCharacter(const FObjectInitializer& ObjectInitializer);

	/* Camera Components */
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<class USphereComponent> CameraOriginComp;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<class USpringArmComponent> CameraArmComp;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<class UCameraComponent> CameraComp;

	/* Movement Functions */
	UFUNCTION()
	void ForwardMovement(float Value);

	UFUNCTION()
	void LateralMovement(float Value);

	/* Camera Functions */
	float CameraPitch, CameraYaw;
	float CameraOrbitSpeed;

	UFUNCTION()
	void OrbitCameraX(float Value);

	UFUNCTION()
	void OrbitCameraY(float Value);

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
