// WarioWare mod by Headshotnoby

#include <engine/shared/config.h>
#include "parachute.h"

MGParachute::MGParachute(CGameContext* pGameServer, CGameControllerWarioWare* pController) : Microgame(pGameServer, pController)
{
	m_microgameName = "parachute";
	m_boss = false;
}

void MGParachute::Start()
{
	for (int i=0; i<MAX_CLIENTS; i++)
	{
		CCharacter *Char = GameServer()->GetPlayerChar(i);
		if (not Char) continue;

		Char->SetHookOthers(false);
		Char->SetHitOthers(false);
		Char->SetCollideOthers(false);
		Char->Core()->m_Jumped = 0; // reset double jump just in case...
		Controller()->teleportPlayer(i, 4);
	}

	GameServer()->SendBroadcast("Get on the platform!", -1);
	Controller()->setPlayerTimers(g_Config.m_WwSndMgGetOnPlatform2_Offset, g_Config.m_WwSndMgGetOnPlatform2_Length);
}

void MGParachute::End()
{
	for (int i=0; i<MAX_CLIENTS; i++)
	{
		CCharacter *Char = GameServer()->GetPlayerChar(i);
		if (not Char) continue;

		Controller()->teleportPlayerToSpawn(i);
	}
}

void MGParachute::Tick()
{
	// nothing to tick
}