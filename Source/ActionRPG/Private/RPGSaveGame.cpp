#include "RPGSaveGame.h"

URPGSaveGame::URPGSaveGame() {
	SaveDataVersion = ERPGSaveGameVersion::LatestVersion;
}

void URPGSaveGame::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	// TODO ���ԭ�������Ƿ�����
}
