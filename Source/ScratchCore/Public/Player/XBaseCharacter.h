#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "XBaseCharacter.generated.h"

UCLASS()
class SCRATCHCORE_API AXBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AXBaseCharacter(const FObjectInitializer& ObjectInitializer);

	/* BaseCharacter | Custom Components */
	UFUNCTION(BlueprintPure)
	FORCEINLINE class UXCharacterMovementComponent* GetCustomCharacterMovement() const 
	{ 
		return MovementComponent; 
	}

	/* BaseCharacter | Camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class UXPlayerCamera> TPCamera = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class USpringArmComponent> CameraArm = nullptr;

	/* BaseCharacter | Movement */
	UFUNCTION()
	void VerticalMovement(float Value);

	UFUNCTION()
	void LateralMovement(float Value);

	UFUNCTION()
	void JumpAction();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	TObjectPtr<class UXCharacterMovementComponent> MovementComponent;

private:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
