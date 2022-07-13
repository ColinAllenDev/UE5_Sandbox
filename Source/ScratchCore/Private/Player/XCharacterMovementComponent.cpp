#include "Player/XCharacterMovementComponent.h"
#include "Player/XBaseCharacter.h"
#include "Player/XPlayerCamera.h"
#include "Player/XPlayerCameraManager.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimInstance.h"
#include "Math/RotationMatrix.h"

UXCharacterMovementComponent::UXCharacterMovementComponent(const FObjectInitializer& ObjectInitializer) 
{
}

void UXCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	AnimInstance = GetCharacterOwner()->GetMesh()->GetAnimInstance();

	// Ignore character owner when detecting climable surfaces
	ClimbQueryParams.AddIgnoredActor(GetOwner());
}

void UXCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Every frame, sweep for surfaces around the player
	// TODO: Possibly toggle this only once player toggles climbing input
	SweepAndStoreWallHits();

	// If enabled, draws debug visuals in editor
	if (bDrawDebug) {
		DrawCollisionDebug();
	}
}

void UXCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) 
{
	// Switch to climbing movement mode if allowed
	if (bWantsToClimb) 
	{
		SetMovementMode(EMovementMode::MOVE_Custom, ECustomMovementMode::CMOVE_Climbing);
	}

	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
}

void UXCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	if (IsClimbing()) 
	{
		bOrientRotationToMovement = false;

		// Shrink capsule to proper size while climbing
		TObjectPtr<UCapsuleComponent> ShrinkCapsule = CharacterOwner->GetCapsuleComponent();
		ShrinkCapsule->SetCapsuleHalfHeight(ShrinkCapsule->GetUnscaledCapsuleHalfHeight() - CollisionCapsuleShrinkAmount);
	}

	const bool bWasClimbing = PreviousMovementMode == MOVE_Custom && PreviousCustomMode == CMOVE_Climbing;
	if (bWasClimbing) 
	{
		// Handle character orientation when returning to grounded state
		bOrientRotationToMovement = true;

		const FRotator StandRotation = FRotator(0, UpdatedComponent->GetComponentRotation().Yaw, 0);
		UpdatedComponent->SetRelativeRotation(StandRotation);

		// Return capsule to it's original size while standing
		TObjectPtr<UCapsuleComponent> StandCapsule = CharacterOwner->GetCapsuleComponent();
		StandCapsule->SetCapsuleHalfHeight(StandCapsule->GetUnscaledCapsuleHalfHeight() + CollisionCapsuleShrinkAmount);

		// Once grounded, reset velocity and acceleration
		StopMovementImmediately();
	}

	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
}

void UXCharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	/* Climbing Physics */
	if (CustomMovementMode == ECustomMovementMode::CMOVE_Climbing)
	{
		PhysClimbing(deltaTime, Iterations);
	}

	Super::PhysCustom(deltaTime, Iterations);
}

float UXCharacterMovementComponent::GetMaxSpeed() const
{
	return IsClimbing() ? MaxClimbingSpeed : Super::GetMaxSpeed();
}

float UXCharacterMovementComponent::GetMaxAcceleration() const
{
	return IsClimbing() ? MaxClimbingAcceleration : Super::GetMaxAcceleration();
}

void UXCharacterMovementComponent::VerticalMovement(FVector CameraForward, float Value)
{	
	FVector Direction;
	if (IsClimbing()) /* Vertical Movement | Climbing */
	{
		Direction = FVector::CrossProduct(GetClimbSurfaceNormal(), -GetCharacterOwner()->GetActorRightVector());
	} 
	else /* Vertical Movement | Grounded */
	{
		Direction = CameraForward;
	}

	GetCharacterOwner()->AddMovementInput(Direction, Value);
}

void UXCharacterMovementComponent::LateralMovement(FVector CameraRight, float Value)
{	

	FVector Direction;
	if (IsClimbing()) /* Lateral Movement | Climbing */
	{
		Direction = FVector::CrossProduct(GetClimbSurfaceNormal(), GetCharacterOwner()->GetActorUpVector());
	}
	else /* Lateral Movement | Grounded */
	{
		Direction = CameraRight;
	}

	GetCharacterOwner()->AddMovementInput(Direction, Value);
}

void UXCharacterMovementComponent::JumpAction()
{
	// Temporary logic
	GetCharacterOwner()->Jump();
}

void UXCharacterMovementComponent::StartClimbing()
{
	if (CanStartClimbing())
	{
		bWantsToClimb = true;
	}
}

void UXCharacterMovementComponent::StopClimbing()
{
	bWantsToClimb = false;
}

FVector UXCharacterMovementComponent::GetClimbSurfaceNormal() const
{
	return CurrentClimbingNormal;
}

bool UXCharacterMovementComponent::IsClimbing() const
{
	return MovementMode == EMovementMode::MOVE_Custom && CustomMovementMode == ECustomMovementMode::CMOVE_Climbing;
}

FQuat UXCharacterMovementComponent::GetClimbingRotation(float deltaTime) const
{
	const FQuat Current = UpdatedComponent->GetComponentQuat();

	if (HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity())
	{
		return Current;
	}

	const FQuat Target = FRotationMatrix::MakeFromX(-CurrentClimbingNormal).ToQuat();

	return FMath::QInterpTo(Current, Target, deltaTime, ClimbingRotationSpeed);
}

void UXCharacterMovementComponent::PhysClimbing(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME) return;

	ComputeSurfaceInfo();

	if (ShouldStopClimbing() || ClimbDownToFloor()) 
	{
		StopClimbing(deltaTime, Iterations);
		return;
	}

	ComputeClimbingVelocity(deltaTime);

	const FVector OldLocation = UpdatedComponent->GetComponentLocation();

	MoveAlongClimbingSurface(deltaTime);

	TryClimbUpLedge();

	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity()) 
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / deltaTime;
	}

	SnapToClimbingSurface(deltaTime);
}

void UXCharacterMovementComponent::ComputeClimbingVelocity(float deltaTime)
{
	RestorePreAdditiveRootMotionVelocity();

	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		constexpr float Friction = 0.0f;
		constexpr bool bFluid = false;
		CalcVelocity(deltaTime, Friction, bFluid, BrakingDecelerationClimbing);
	}

	ApplyRootMotionToVelocity(deltaTime);
}

void UXCharacterMovementComponent::MoveAlongClimbingSurface(float deltaTime)
{
	const FVector Adjusted = Velocity * deltaTime;

	FHitResult Hit(1.f);

	SafeMoveUpdatedComponent(Adjusted, GetClimbingRotation(deltaTime), true, Hit);

	if (Hit.Time < 1.f)
	{
		HandleImpact(Hit, deltaTime, Adjusted);
		SlideAlongSurface(Adjusted, (1.f - Hit.Time), Hit.Normal, Hit, true);
	}
}

void UXCharacterMovementComponent::SnapToClimbingSurface(float deltaTime) const
{
	const FVector Forward = UpdatedComponent->GetForwardVector();
	const FVector Location = UpdatedComponent->GetComponentLocation();
	const FQuat Rotation = UpdatedComponent->GetComponentQuat();

	const FVector ForwardDifference = (CurrentClimbingPosition - Location).ProjectOnTo(Forward);
	const FVector Offset = -CurrentClimbingNormal * (ForwardDifference.Length() - DistanceFromSurface);

	constexpr bool bSweep = true;
	UpdatedComponent->MoveComponent(Offset * ClimbingSnapSpeed * deltaTime, Rotation, bSweep);
}

void UXCharacterMovementComponent::StopClimbing(float deltaTime, int32 Iterations)
{
	bWantsToClimb = false;
	SetMovementMode(EMovementMode::MOVE_Falling);
	StartNewPhysics(deltaTime, Iterations);
}

void UXCharacterMovementComponent::SweepAndStoreWallHits()
{
	const FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(CollisionCapsuleRadius, CollisionCapsuleHalfHeight);
	const FVector StartOffset = UpdatedComponent->GetForwardVector() * 20;

	const FVector Start = UpdatedComponent->GetComponentLocation() + StartOffset;
	const FVector End = Start + UpdatedComponent->GetForwardVector();

	TArray<FHitResult> Hits;
	const bool HitWall = GetWorld()->SweepMultiByChannel(Hits, Start, End,
		FQuat::Identity, ECC_WorldStatic, CollisionShape, ClimbQueryParams);

	HitWall ? CurrentWallHits = Hits : CurrentWallHits.Reset();
}

void UXCharacterMovementComponent::ComputeSurfaceInfo()
{
	CurrentClimbingNormal = FVector::ZeroVector;
	CurrentClimbingPosition = FVector::ZeroVector;

	if (CurrentWallHits.IsEmpty()) return;

	const FVector Start = UpdatedComponent->GetComponentLocation();
	const FCollisionShape CollisionSphere = FCollisionShape::MakeSphere(6);

	for (const FHitResult& WallHit : CurrentWallHits)
	{
		const FVector End = Start + (WallHit.ImpactPoint - Start).GetSafeNormal() * 120;

		FHitResult AssistHit;
		GetWorld()->SweepSingleByChannel(AssistHit, Start, End, FQuat::Identity,
			ECC_WorldStatic, CollisionSphere, ClimbQueryParams);

		CurrentClimbingNormal += AssistHit.Normal;
		CurrentClimbingPosition += AssistHit.ImpactPoint;
	}

	CurrentClimbingNormal = CurrentClimbingNormal.GetSafeNormal();
	CurrentClimbingPosition /= CurrentWallHits.Num();
}

bool UXCharacterMovementComponent::CheckFloor(FHitResult& FloorHit) const
{
	const FVector Start = UpdatedComponent->GetComponentLocation();
	const FVector End = Start + FVector::DownVector * FloorCheckDistance;

	return GetWorld()->LineTraceSingleByChannel(FloorHit, Start, End, ECC_WorldStatic, ClimbQueryParams);
}

bool UXCharacterMovementComponent::EyeHeightTrace(const float TraceDistance) const
{
	FHitResult UpperEdgeHit;

	const float BaseEyeHeight = GetCharacterOwner()->BaseEyeHeight;
	const float EyeHeightOffset = IsClimbing() ? BaseEyeHeight + CollisionCapsuleShrinkAmount : BaseEyeHeight;

	const FVector Start = UpdatedComponent->GetComponentLocation() + UpdatedComponent->GetUpVector() * EyeHeightOffset;

	const FVector End = Start + (UpdatedComponent->GetForwardVector() * TraceDistance);

	return GetWorld()->LineTraceSingleByChannel(UpperEdgeHit, Start, End, ECC_WorldStatic, ClimbQueryParams);
}

bool UXCharacterMovementComponent::IsFacingSurface(const float SurfaceVerticalDot) const 
{
	constexpr float BaseLength = 80;
	const float SteepnessMultiplier = 1 + (1 - SurfaceVerticalDot) * 5;

	return EyeHeightTrace(BaseLength * SteepnessMultiplier);
}

bool UXCharacterMovementComponent::HasReachedEdge() const
{
	const TObjectPtr<UCapsuleComponent> Capsule = CharacterOwner->GetCapsuleComponent();
	const float TraceDistance = Capsule->GetUnscaledCapsuleRadius() * 2.5f;

	return !EyeHeightTrace(TraceDistance);
}

bool UXCharacterMovementComponent::IsLocationWalkable(const FVector& CheckLocation) const
{
	const FVector CheckEnd = CheckLocation + (FVector::DownVector * 250.f);

	FHitResult LedgeHit;
	const bool bHitLedgeGround = GetWorld()->LineTraceSingleByChannel(LedgeHit, CheckLocation,
		CheckEnd, ECC_WorldStatic, ClimbQueryParams);

	return bHitLedgeGround && LedgeHit.Normal.Z >= GetWalkableFloorZ();
}

bool UXCharacterMovementComponent::CanMoveToLedgeClimbLocation() const
{
	const FVector VerticalOffset = FVector::UpVector * LedgeCheckVerticalOffset;
	const FVector HorizontalOffset = UpdatedComponent->GetForwardVector() * LedgeCheckHorizontalOffset;

	const FVector CheckLocation = UpdatedComponent->GetComponentLocation() + HorizontalOffset + VerticalOffset;

	if (!IsLocationWalkable(CheckLocation)) return false;

	FHitResult CapsuleHit;
	const FVector CapsuleStartCheck = CheckLocation - HorizontalOffset;
	const TObjectPtr<UCapsuleComponent> Capsule = CharacterOwner->GetCapsuleComponent();

	const bool bBlocked = GetWorld()->SweepSingleByChannel(CapsuleHit, CapsuleStartCheck, CheckLocation,
		FQuat::Identity, ECC_WorldStatic, Capsule->GetCollisionShape(), ClimbQueryParams);

	return !bBlocked;
}

bool UXCharacterMovementComponent::CanStartClimbing() 
{
	for (FHitResult& Hit : CurrentWallHits) 
	{
		const FVector HorizontalNormal = Hit.Normal.GetSafeNormal2D();

		const float HorizontalDot = FVector::DotProduct(UpdatedComponent->GetForwardVector(), -HorizontalNormal);
		const float VerticalDot = FVector::DotProduct(Hit.Normal, HorizontalNormal);

		const float HorizontalDegrees = FMath::RadiansToDegrees(FMath::Acos(HorizontalDot));
		
		const bool bIsCeiling = FMath::IsNearlyZero(VerticalDot);

		if (HorizontalDegrees <= MinStartClimbingAngle && !bIsCeiling && IsFacingSurface(VerticalDot)) 
		{
			return true;
		}
	}
	return false;
}

bool UXCharacterMovementComponent::ShouldStopClimbing()
{
	const bool bIsOnCeiling = FVector::Parallel(CurrentClimbingNormal, FVector::UpVector);

	return !bWantsToClimb || CurrentClimbingNormal.IsZero() || bIsOnCeiling;
}

bool UXCharacterMovementComponent::ClimbDownToFloor() const
{
	FHitResult FloorHit;

	if (!CheckFloor(FloorHit))
	{
		return false;
	}

	const bool bOnWalkableFloor = FloorHit.Normal.Z > GetWalkableFloorZ();
	
	const float DownSpeed = FVector::DotProduct(Velocity, -FloorHit.Normal);
	const bool bIsMovingTowardsFloor = DownSpeed >= MaxClimbingSpeed / 3 && bOnWalkableFloor;

	const bool bIsClimbingFloor = CurrentClimbingNormal.Z > GetWalkableFloorZ();

	return bIsMovingTowardsFloor || (bIsClimbingFloor && bOnWalkableFloor);
}

bool UXCharacterMovementComponent::TryClimbUpLedge() const
{
	// If anim montage is already playing, return.
	if (AnimInstance && LedgeClimbMontage && AnimInstance->Montage_IsPlaying(LedgeClimbMontage)) 
	{
		return false;
	}

	const float UpSpeed = FVector::DotProduct(Velocity, UpdatedComponent->GetUpVector());
	const bool bIsMovingUp = UpSpeed >= MaxClimbingSpeed / 3;

	if (bIsMovingUp && HasReachedEdge() && CanMoveToLedgeClimbLocation()) 
	{
		const FRotator StandRotation = FRotator(0, UpdatedComponent->GetComponentRotation().Yaw, 0);
		UpdatedComponent->SetRelativeRotation(StandRotation);

		AnimInstance->Montage_Play(LedgeClimbMontage);

		return true;
	}

	return false;
}

/* Debugging */
void UXCharacterMovementComponent::DrawCollisionDebug()
{
	DrawDebugCapsule(GetWorld(), UpdatedComponent->GetComponentLocation(), 
				CollisionCapsuleHalfHeight, CollisionCapsuleRadius, FQuat::Identity, FColor::White);



	if (!CurrentWallHits.IsEmpty()) 
	{
		for (FHitResult& Hit : CurrentWallHits) 
		{
			DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 7.5f, 12, FColor::Blue);
		}
	}
}