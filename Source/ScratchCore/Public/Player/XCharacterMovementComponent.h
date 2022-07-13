#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "XCharacterMovementComponent.generated.h"

UENUM(BlueprintType)
enum ECustomMovementMode 
{
	CMOVE_Climbing	UMETA(DisplayName = "Climbing"),
	CMOVE_MAX		UMETA(Hidden),
};

/**
 * XCharacterMovementComponent handles movement logic for the associated character owner.
 * It is a custom implementation of the standard UCharacterMovementComponent in UE5.
 * It (currently) handles walking/running, jumping, and climbing.
 */
UCLASS()
class SCRATCHCORE_API UXCharacterMovementComponent : public UCharacterMovementComponent {
	GENERATED_BODY()

public:
	/** Default constructor */
	UXCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);

private:
	/** 
	 * Functions overriden from UCharacterMovementComponent 
	 */
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	virtual float GetMaxSpeed() const override;
	virtual float GetMaxAcceleration() const override;

public:

	/* ======== LOCOMOTION SYSTEM ========= */
	/* === Locomotion System | Movement === */
	
	/** 
	 * Handles movement in the forward/backward direction.
	 * @param CameraForward - The camera's forward vector
	 * @param Value - The magnitude of our given input
	 */
	void VerticalMovement(FVector CameraForward, float Value);
	
	/**
	 * Handles movement in the right/left direction.
	 * @param CameraForward - The camera's right vector
	 * @param Value - The magnitude of our given input
	 */
	void LateralMovement(FVector CameraRight, float Value);
	
	/** Jumping action toggled by input */
	void JumpAction();

	/* ========= CLIMBING SYSTEM ======== */
	/* === Climbing System | Movement === */

	/** Attempt to start climbing if allowed. @see CanStartClimbing */
	void StartClimbing();

	/** Toggle climbing to stop. */
	void StopClimbing();

	/* ==== Climbing System | Collision ==== */

	/** @returns a vector of the surface normal of the surface we are attempting to climb. */
	UFUNCTION(BlueprintPure)
	FVector GetClimbSurfaceNormal() const;

	/* ==== Climbing System | Logic ==== */

	/** Represents if the player is able to climb based on the output of CanStartClimbing. */
	bool bWantsToClimb = false;

	/** @returns true if the current movement mode is our custom CMOVE_Climbing mode. @see ECustomMovementMode*/
	UFUNCTION(BlueprintPure)
	bool IsClimbing() const;

private:
	/* ========= CLIMBING SYSTEM ======== */
	/* === Climbing System | Movement === */
	
	/** The top speed at which the player can climb. */
	UPROPERTY(EditAnywhere, meta = (ClampMin = "10.0", ClampMax = "500.0"), Category = "Climbing System | Movement")
	float MaxClimbingSpeed = 120.f;

	/** The top rate of acceleration while climbing. */
	UPROPERTY(EditAnywhere, meta = (ClampMin = "10.0", ClampMax = "2000.0"), Category = "Climbing System | Movement")
	float MaxClimbingAcceleration = 300.f;

	/** The top rate of deceleration while climbing. */
	UPROPERTY(EditAnywhere, meta = (ClampMin = "10.0", ClampMax = "3000.0"), Category = "Climbing System | Movement")
	float BrakingDecelerationClimbing = 550.f;

	/** The top speed at which the player can rotate around surface. */
	UPROPERTY(EditAnywhere, meta = (ClampMin="1.0", ClampMax = "12.0"), Category = "Climbing System | Movement")
	int ClimbingRotationSpeed = 6;

	/** The speed at which the player "snaps" to a given surface when transitioning from a non-climbing state. */
	UPROPERTY(EditAnywhere, meta = (ClampMin = "1.0", ClampMax = "60.0"), Category = "Climbing System | Movement")
	float ClimbingSnapSpeed = 4.f;
	
	/** The offset of the player from the surface along it's normal. */
	UPROPERTY(EditAnywhere, meta = (ClampMin = "1.0", ClampMax = "80.0"), Category = "Climbing System | Movement")
	float DistanceFromSurface = 45.f;

	/** Quaternion representing the current rotation of the player interpolated to the climable surface normal. */
	FQuat GetClimbingRotation(float deltaTime) const;

	/** 
	 * The custom physics system used when in CMOVE_Climbing movement mode. 
	 * @param deltaTime - frame time to advance, in seconds
	 * @param Iterations - current iteration of the tick (starting at 1).
	 */
	void PhysClimbing(float deltaTime, int32 Iterations);

	/** Computes the player's velocity while climbing, considering friction, acceleration and decelertaion. */
	void ComputeClimbingVelocity(float deltaTime);

	/** The player movement along a climable surface while in the climbing state. */
	void MoveAlongClimbingSurface(float deltaTime);

	/** Snaps the player to a climable surface when transitioning from a grounded or in-air state. */
	void SnapToClimbingSurface(float deltaTime) const;

	/**
	 * Stops the player from climbing by changing the movement mode to MOVE_Falling and starting a new physics simulation.
	 * @param deltaTime - frame time to advance, in seconds
	 * @param Iterations - current iteration of the tick (starting at 1).
	 */
	void StopClimbing(float deltaTime, int32 Iterations);

	/* === Climbing System | Collision === */

	/** The minimum valid angle between the player's forward vector and the surface normal to allow climbing */
	UPROPERTY(EditAnywhere, meta=(ClampMin="1.0", ClampMax="75.0"), Category="Climbing System | Collision")
	float MinStartClimbingAngle = 25.0f;

	/** The radius of the collision capsule used to detect climable surfaces around the player. */
	UPROPERTY(EditAnywhere, Category = "Climbing System | Collision")
	int CollisionCapsuleRadius = 50;
	
	/** The half-height of the collision capsule used to detect climable surfaces around the player. */
	UPROPERTY(EditAnywhere, Category = "Climbing System | Collision")
	int CollisionCapsuleHalfHeight = 72;

	/** The amount by which the collision capsule must shrink while climbing. */
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "80.0"), Category="Climbing System | Collision")
	float CollisionCapsuleShrinkAmount = 30;

	/** The minimum distance the player must be from the floor to dismount into a standing position */
	UPROPERTY(EditAnywhere, meta = (ClampMin = "1.0", ClampMax = "500.0"), Category = "Climbing System | Collision")
	float FloorCheckDistance = 100.f;

	/** Array storing current FHitResults found while sweeping for climable surfaces. @see SweepAndStoreWallHits */
	TArray<FHitResult> CurrentWallHits;

	/** Collision query parameters used when detecting climable surfaces. */
	FCollisionQueryParams ClimbQueryParams;

	/** Vector representing the normal of the climable surface. @see GetClimbSurfaceNormal */
	FVector CurrentClimbingNormal;
	/** Vector representing the impact point of the climable surface. @see ComputeSurfaceInfo */
	FVector CurrentClimbingPosition;

	/** Using a capsule collider, sweep for surfaces around the player populate CurrentWallHits. */
	void SweepAndStoreWallHits();

	/** Computes a climable surfaces' normal and impact position and updates CurrentClimbingNormal and CurrentClimbingPosition. */
	void ComputeSurfaceInfo();

	/** @returns true when there is a floor beneath the player. */
	bool CheckFloor(FHitResult& FloorHit) const;

	/* === Climbing System | Logic === */
	
	UPROPERTY(EditAnywhere, Category = "Climbing System | Ledges")
	float LedgeCheckVerticalOffset = 160.f;

	UPROPERTY(EditAnywhere, Category = "Climbing System | Ledges")
	float LedgeCheckHorizontalOffset = 120.f;

	/** 
	 * @returns true if the player's eyes are inline with a climable wall.
	 * @param TraceDistance - the forward distance from which the player is able to detect walls.
	 */
	bool EyeHeightTrace(const float TraceDistance) const;
	
	/** 
	 * @returns true if the player is facing a climable surface.
	 * @param SurfaceVerticalDot - the dot product of the player's normal and the climable surface's normal.
	 */
	bool IsFacingSurface(const float SurfaceVerticalDot) const;

	/** @returns true if EyeHeightTrace doesn't find a climable surface at eye height. */
	bool HasReachedEdge() const;

	/** */
	bool IsLocationWalkable(const FVector& CheckLocation) const;
	
	/** */
	bool CanMoveToLedgeClimbLocation() const;
	
	/** @returns true if the player can start climbing a given surface. */
	bool CanStartClimbing();
	
	/** @returns true if the player no longer wants to climb or is attempting to climb an invalid surface. */
	bool ShouldStopClimbing();

	/** @returns true if the player is able to move from a climbing state to a grounded state. */
	bool ClimbDownToFloor() const;

	/** @returns true if a player is able to mount over a ledge. */
	bool TryClimbUpLedge() const;

	/* === Climbing System | Animation === */

	/** AnimMontage component used when mounting ledges from a climbing state. */
	UPROPERTY(EditDefaultsOnly, Category = "Climbing System | Animation")
	TObjectPtr<class UAnimMontage> LedgeClimbMontage;

	/** The player character's AnimInstance component. */
	UPROPERTY()
	TObjectPtr<class UAnimInstance> AnimInstance;

private:
	/* ======== DEBUGGING ======== */
	
	/** @if true, draw debug visuals in editor */
	UPROPERTY(EditAnywhere, Category = "Debugging")
	bool bDrawDebug = false;

	/** Draws debug visualizations relevant to the climbing system */
	void DrawCollisionDebug();
};