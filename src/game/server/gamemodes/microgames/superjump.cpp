// WarioWare mod by Headshotnoby

#include <engine/shared/config.h>
#include "superjump.h"

MGSuperJump::MGSuperJump(CGameContext* pGameServer, CGameControllerWarioWare* pController) : Microgame(pGameServer, pController)
{
	m_microgameName = "superjump";
	m_boss = false;
}

void MGSuperJump::Start()
{
	for (int i=0; i<MAX_CLIENTS; i++)
	{
		CCharacter *Char = GameServer()->GetPlayerChar(i);
		if (not Char) continue;

		Char->SetHookOthers(false);
		Char->SetCollideOthers(false);
		Char->SetSuperJump(true);
	}

	GameServer()->SendBroadcast("Touch the sky!", -1);
	Controller()->setPlayerTimers(g_Config.m_WwSndMgTouchSky_Offset, g_Config.m_WwSndMgTouchSky_Length);
}

void MGSuperJump::End()
{
	for (int i=0; i<MAX_CLIENTS; i++)
	{
		CCharacter *Char = GameServer()->GetPlayerChar(i);
		if (not Char) continue;

		Controller()->teleportPlayerToSpawn(i);
	}
}

void MGSuperJump::Tick()
{
	// nothing to tick
}