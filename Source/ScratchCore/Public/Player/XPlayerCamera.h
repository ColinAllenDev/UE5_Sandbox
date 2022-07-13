#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "XPlayerCamera.generated.h"

UCLASS()
class SCRATCHCORE_API UXPlayerCamera : public UCameraComponent {
	GENERATED_BODY()
public:
	UXPlayerCamera(const FObjectInitializer& ObjectInitializer);
};