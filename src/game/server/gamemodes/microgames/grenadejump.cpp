// WarioWare mod by Headshotnoby

#include <engine/shared/config.h>
#include "grenadejump.h"

MGGrenadeJump::MGGrenadeJump(CGameContext* pGameServer, CGameControllerWarioWare* pController) : Microgame(pGameServer, pController)
{
	m_microgameName = "grenadejump";
	m_boss = false;
}

void MGGrenadeJump::Start()
{
	GameServer()->SendBroadcast("Grenade jump!", -1);
	Controller()->setPlayerTimers(g_Config.m_WwSndMgGrenadeJump_Offset, g_Config.m_WwSndMgGrenadeJump_Length);
	
	for (int i=0; i<MAX_CLIENTS; i++)
	{
		CCharacter *Char = GameServer()->GetPlayerChar(i);
		if (not Char) continue;
		
		Char->GiveWeapon(WEAPON_GRENADE, -1);
		Char->SetWeapon(WEAPON_GRENADE);
	}
}

void MGGrenadeJump::End()
{
	for (int i=0; i<MAX_CLIENTS; i++)
	{
		CCharacter *Char = GameServer()->GetPlayerChar(i);
		if (not Char) continue;

		Char->SetWeaponGot(WEAPON_GRENADE, false);
		if (Char->GetActiveWeapon() == WEAPON_GRENADE and Char->GetLastWeapon() != WEAPON_GRENADE)
			Char->SetWeapon(Char->GetLastWeapon());
	}
}

void MGGrenadeJump::Tick()
{
	for (int i=0; i<MAX_CLIENTS; i++)
	{
		CCharacter *Char = GameServer()->GetPlayerChar(i);
		if (not Char) continue;

		if (Char->Core()->m_Vel.y <= -18 and Char->GetActiveWeapon() == WEAPON_GRENADE)
			Controller()->winMicroGame(i);
	}
}