// WarioWare mod by Headshotnoby

#include <engine/shared/config.h>
#include "train.h"

MGTrain::MGTrain(CGameContext* pGameServer, CGameControllerWarioWare* pController) : Microgame(pGameServer, pController)
{
	m_microgameName = "train";
	m_boss = false;
}

void MGTrain::Start()
{
	for (int i=0; i<MAX_CLIENTS; i++)
	{
		CCharacter *Char = GameServer()->GetPlayerChar(i);
		if (not Char) continue;
		
		Char->SetHookOthers(false);
		Char->SetHitOthers(false);
		Char->SetCollideOthers(false);
		Controller()->teleportPlayer(i, 3);
	}
	
	GameServer()->SendBroadcast("Avoid the train!", -1);
	Controller()->setPlayerTimers(g_Config.m_WwSndMgAvoidTrain_Offset, g_Config.m_WwSndMgAvoidTrain_Length);
}

void MGTrain::End()
{
	for (int i=0; i<MAX_CLIENTS; i++)
	{
		CCharacter *Char = GameServer()->GetPlayerChar(i);
		if (not Char) continue;

		Controller()->teleportPlayerToSpawn(i);
	}
}

void MGTrain::Tick()
{
	// nothing to tick
}