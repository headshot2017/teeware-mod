// WarioWare mod by Headshotnoby

#include <engine/shared/config.h>
#include "blockfight.h"

MGBlockFight::MGBlockFight(CGameContext* pGameServer, CGameControllerWarioWare* pController) : Microgame(pGameServer, pController)
{
	m_microgameName = "blockfight";
	m_boss = false;
}

void MGBlockFight::Start()
{
	for (int i=0; i<MAX_CLIENTS; i++)
	{
		if (not GameServer()->GetPlayerChar(i)) continue;

		Controller()->g_Complete[i] = true;
		Controller()->teleportPlayer(i, 2);
	}
	
	GameServer()->SendBroadcast("Fight and stay alive!", -1);
	Controller()->setPlayerTimers(g_Config.m_WwSndMgFightStayAlive_Offset, g_Config.m_WwSndMgFightStayAlive_Length);
}

void MGBlockFight::End()
{
	for (int i=0; i<MAX_CLIENTS; i++)
	{
		CCharacter *Char = GameServer()->GetPlayerChar(i);
		if (not Char) continue;

		Controller()->teleportPlayerToSpawn(i);
	}
}

void MGBlockFight::Tick()
{
	// nothing to tick
}

int MGBlockFight::OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon)
{
	Controller()->g_Complete[pVictim->GetPlayer()->GetCID()] = false;
	return 0;
}