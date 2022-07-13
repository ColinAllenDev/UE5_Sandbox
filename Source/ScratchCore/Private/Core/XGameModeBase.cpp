#include "Core/XGameModeBase.h"
#include "Core/XGameStateBase.h"
#include "Player/XPlayerController.h"
#include "Player/XBaseCharacter.h"

AXGameModeBase::AXGameModeBase() {
	/* GameMode Defaults */
	PlayerControllerClass = AXPlayerController::StaticClass();
	DefaultPawnClass = AXBaseCharacter::StaticClass();
}

void AXGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) {
	Super::InitGame(MapName, Options, ErrorMessage);
}

void AXGameModeBase::StartPlay() {
	/* Initialization Phase */
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("XGameMode :: StartPlay called"));

	Super::StartPlay();
}
