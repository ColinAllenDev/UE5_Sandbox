#include "Player/XPlayerController.h"
#include "Player/XPlayerCameraManager.h"
#include "Player/XBaseCharacter.h"
#include "Player/XCharacterMovementComponent.h"
#include "Components/InputComponent.h"

AXPlayerController::AXPlayerController() 
{
	PlayerCameraManagerClass = AXPlayerCameraManager::StaticClass();
}

void AXPlayerController::SetupInputComponent() 
{
	Super::SetupInputComponent();

	// Assertation the input component exists
	check(InputComponent != nullptr);

	/* Input Bindings */
	InputComponent->BindAxis("Movement_Vertical", this, &AXPlayerController::ForwardMovement);
	InputComponent->BindAxis("Movement_Lateral", this, &AXPlayerController::LateralMovement);
	InputComponent->BindAction("Jump_Toggle", IE_Pressed, this, &AXPlayerController::JumpAction);

	InputComponent->BindAxis("Camera_OrbitX", this, &AXPlayerController::OrbitCameraYaw);
	InputComponent->BindAxis("Camera_OrbitY", this, &AXPlayerController::OrbitCameraPitch);
	
	InputComponent->BindAction("Climb_Toggle", IE_Pressed, this, &AXPlayerController::StartClimbing);
	//InputComponent->BindAction("Climb_Toggle", IE_Released, this, &AXPlayerController::StopClimbing);
}

void AXPlayerController::OnPossess(class APawn* PawnToPossess)
{
	Super::OnPossess(PawnToPossess);
}

void AXPlayerController::BeginPlay() 
{
	Super::BeginPlay();

	/* Pawn Setup */
	PlayerCharacterRef = Cast<AXBaseCharacter>(GetPawn());
	UE_LOG(LogTemp, Warning, TEXT("XPlayerController :: Possessing Pawn %s"), *GetPawn()->GetName());

	/* Camera Setup */
	PlayerCameraManagerRef = Cast<AXPlayerCameraManager>(PlayerCameraManager); // TODO: Maybe move this?
	if (PlayerCharacterRef && PlayerCameraManagerRef) 
	{
		PlayerCameraManagerRef->OnPossess(PlayerCharacterRef);
	}
}

void AXPlayerController::AdjustGroundedOrientation()
{
	float vMag = InputComponent->GetAxisValue("Movement_Vertical");
	float lMag = InputComponent->GetAxisValue("Movement_Lateral");

	UXCharacterMovementComponent* CharacterMovement = PlayerCharacterRef->GetCustomCharacterMovement();

	if (CharacterMovement->MovementMode == EMovementMode::MOVE_Walking) 
	{
		if (vMag != 0 || lMag != 0)
		{
			CharacterMovement->bOrientRotationToMovement = true;
		}
		else
		{
			CharacterMovement->bOrientRotationToMovement = false;
		}
	}
}

void AXPlayerController::ForwardMovement(float Value) 
{
	AdjustGroundedOrientation();

	PlayerCharacterRef->VerticalMovement(Value);
}

void AXPlayerController::LateralMovement(float Value) 
{
	AdjustGroundedOrientation();
	
	PlayerCharacterRef->LateralMovement(Value);
}

void AXPlayerController::JumpAction() {
	PlayerCharacterRef->JumpAction();
}

void AXPlayerController::StartClimbing() 
{
	PlayerCharacterRef->GetCustomCharacterMovement()->StartClimbing();
}

void AXPlayerController::StopClimbing() 
{
	PlayerCharacterRef->GetCustomCharacterMovement()->StopClimbing();
}

void AXPlayerController::OrbitCameraYaw(float Value) 
{
	PlayerCameraManagerRef->OrbitCameraYaw(Value);
}

void AXPlayerController::OrbitCameraPitch(float Value) 
{
	PlayerCameraManagerRef->OrbitCameraPitch(Value);
}

