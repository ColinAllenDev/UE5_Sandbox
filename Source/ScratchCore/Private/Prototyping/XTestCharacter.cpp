#include "Prototyping/XTestCharacter.h"
#include "Player/XCharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "Components/SphereComponent.h" 
#include "Camera/CameraComponent.h"
#include "Math/RotationMatrix.h"


AXTestCharacter::AXTestCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UXCharacterMovementComponent>
	(CharacterMovementComponentName)) {
	PrimaryActorTick.bCanEverTick = true;

	CameraOrbitSpeed = 1.5f;
	bFindCameraComponentWhenViewTarget = true;

	CameraOriginComp = CreateDefaultSubobject<USphereComponent>("CameraOrigin");
	CameraOriginComp->SetRelativeLocation(FVector(0.0f, 0.0f, 90.0f)); // Maybe get rid of this
	CameraOriginComp->SetupAttachment(GetRootComponent());

	CameraArmComp = CreateDefaultSubobject<USpringArmComponent>("CameraArm");
	CameraArmComp->SetupAttachment(CameraOriginComp);

	CameraComp = CreateDefaultSubobject<UCameraComponent>("Camera");
	CameraComp->SetupAttachment(CameraArmComp);
}

void AXTestCharacter::ForwardMovement(float Value) {
	if (Controller != nullptr && Value != 0.0f) {
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AXTestCharacter::LateralMovement(float Value)
{

}

void AXTestCharacter::OrbitCameraX(float Value)
{
	CameraYaw = Value * CameraOrbitSpeed;

	FRotator DeltaRotation(0.0f, CameraYaw, 0.0f);
	CameraOriginComp->AddRelativeRotation(DeltaRotation);
}

void AXTestCharacter::OrbitCameraY(float Value)
{
	CameraPitch = Value * CameraOrbitSpeed;

	FRotator DeltaRotation(CameraPitch, 0.0f, 0.0f);
	CameraOriginComp->AddRelativeRotation(DeltaRotation);
}

void AXTestCharacter::BeginPlay() {
	Super::BeginPlay();
}

void AXTestCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AXTestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	/* Input Bindings */
	//InputComponent->BindAxis("ForwardMovement", this, &AXTestCharacter::ForwardMovement);
}

