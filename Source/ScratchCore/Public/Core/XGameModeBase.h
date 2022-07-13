#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "XGameModeBase.generated.h"

UCLASS()
class SCRATCHCORE_API AXGameModeBase : public AGameModeBase {
	GENERATED_BODY()
public:
	AXGameModeBase();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	
	virtual void StartPlay() override;

};