#include "ScratchCore.h"
#include "Modules/ModuleManager.h"

#include "Log.h"

void FScratchCore::StartupModule() {
	UE_LOG(LogScratchCore, Log, TEXT("ScratchCore module starting up"));
}

void FScratchCore::ShutdownModule() {
	UE_LOG(LogScratchCore, Log, TEXT("ScratchCore module shutting down"));
}

IMPLEMENT_PRIMARY_GAME_MODULE(FScratchCore, ScratchCore, "ScratchCore");