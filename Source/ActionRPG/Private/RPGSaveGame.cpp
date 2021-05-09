#include "RPGSaveGame.h"

URPGSaveGame::URPGSaveGame() {
	SaveDataVersion = ERPGSaveGameVersion::LatestVersion;
}

void URPGSaveGame::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	// TODO 检查原生代码是否有用
}
