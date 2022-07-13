#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

class FScratchCore : public IModuleInterface {
public:
	static inline FScratchCore& Get() {
		return FModuleManager::LoadModuleChecked<FScratchCore>("ScratchCore");
	}

	static inline bool IsAvailable() {
		return FModuleManager::Get().IsModuleLoaded("ScratchCore");
	}

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};