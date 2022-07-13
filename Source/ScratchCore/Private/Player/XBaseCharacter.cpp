#include "Player/XBaseCharacter.h"
#include "Player/XCharacterMovementComponent.h"
#include "Player/XPlayerCamera.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "Components/CapsuleComponent.h"
#include "Math/RotationMatrix.h"

AXBaseCharacter::AXBaseCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass
		<UXCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	/* Component Initialization */
	MovementComponent = Cast<UXCharacterMovementComponent>(GetCharacterMovement());
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 88.0f);

	/* Camera Initialization */	
	CameraArm = CreateDefaultSubobject<USpringArmComponent>("CameraArm");
	CameraArm->SetupAttachment(RootComponent);
	CameraArm->TargetArmLength = 400.0f;
	CameraArm->bUsePawnControlRotation = true;

	TPCamera = CreateDefaultSubobject<UXPlayerCamera>("TPCamera");
	TPCamera->SetupAttachment(CameraArm, USpringArmComponent::SocketName);
	TPCamera->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	bFindCameraComponentWhenViewTarget = true;
}

void AXBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AXBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AXBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AXBaseCharacter::VerticalMovement(float Value)
{
	check(Controller != nullptr);

	MovementComponent->VerticalMovement(TPCamera->GetForwardVector(), Value);
}

void AXBaseCharacter::LateralMovement(float Value)
{
	check (Controller != nullptr)

	MovementComponent->LateralMovement(TPCamera->GetRightVector(), Value);
}

void AXBaseCharacter::JumpAction()
{
	MovementComponent->JumpAction();
}
