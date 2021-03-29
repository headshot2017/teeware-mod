// WarioWare mod by Headshotnoby

#include <engine/shared/config.h>
#include "gettoend.h"

MGGetToEnd::MGGetToEnd(CGameContext* pGameServer, CGameControllerWarioWare* pController) : Microgame(pGameServer, pController)
{
	m_microgameName = "gettoend";
	m_boss = false;
}

void MGGetToEnd::Start()
{
	for (int i=0; i<MAX_CLIENTS; i++)
	{
		CCharacter *Char = GameServer()->GetPlayerChar(i);
		if (not Char) continue;

		Controller()->teleportPlayer(i, 1);
		Char->SetHookOthers(false);
		Char->SetHitOthers(false);
		Char->SetCollideOthers(false);
	}
	
	GameServer()->SendBroadcast("Get to the end!", -1);
	Controller()->setPlayerTimers(g_Config.m_WwSndMgGetToEnd_Offset, g_Config.m_WwSndMgGetToEnd_Length);
}

void MGGetToEnd::End()
{
	for (int i=0; i<MAX_CLIENTS; i++)
	{
		CCharacter *Char = GameServer()->GetPlayerChar(i);
		if (not Char) continue;

		Controller()->teleportPlayerToSpawn(i);
	}
}

void MGGetToEnd::Tick()
{
	// nothing to tick
}