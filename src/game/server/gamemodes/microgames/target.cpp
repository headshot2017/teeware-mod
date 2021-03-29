// WarioWare mod by Headshotnoby

#include <engine/shared/config.h>
#include "target.h"

MGTarget::MGTarget(CGameContext* pGameServer, CGameControllerWarioWare* pController) : Microgame(pGameServer, pController)
{
	m_microgameName = "target";
	m_boss = false;
}

void MGTarget::Start()
{
	for (int i=0; i<MAX_CLIENTS-1; i++)
	{
		CCharacter *Char = GameServer()->GetPlayerChar(i);
		if (not Char) continue;

		Char->SetHookOthers(false);
		Char->SetHitOthers(false);
		Char->SetCollideOthers(false);
		
		Char->GiveWeapon(WEAPON_RIFLE, -1);
		Char->SetWeapon(WEAPON_RIFLE);
		
		Controller()->teleportPlayer(i, 6);
	}

	// teleport the bot player (the target)
	int bot_tele = 7;
	int Num = Controller()->m_TeleOuts[bot_tele-1].size();
	Server()->SetClientName(MAX_CLIENTS-1, "HIT ME");
	GameServer()->m_apPlayers[MAX_CLIENTS-1]->SetTeam(0, false); // move to game
	GameServer()->m_apPlayers[MAX_CLIENTS-1]->ForceSpawn(Controller()->m_TeleOuts[bot_tele-1][(!Num)?Num:rand() % Num]);
	
	GameServer()->SendBroadcast("Hit the target!", -1);
	Controller()->setPlayerTimers(g_Config.m_WwSndMgHitTheTarget_Offset, g_Config.m_WwSndMgHitTheTarget_Length);
}

void MGTarget::End()
{
	for (int i=0; i<MAX_CLIENTS-1; i++)
	{
		CCharacter *Char = GameServer()->GetPlayerChar(i);
		if (not Char) continue;

		Char->SetWeaponGot(WEAPON_RIFLE, false);
		if (Char->GetActiveWeapon() == WEAPON_RIFLE and Char->GetLastWeapon() != WEAPON_RIFLE)
			Char->SetWeapon(Char->GetLastWeapon());

		Controller()->teleportPlayerToSpawn(i);
	}

	// move bot back to spec
	GameServer()->m_apPlayers[MAX_CLIENTS-1]->SetTeam(TEAM_SPECTATORS, false);
	Server()->SetClientName(MAX_CLIENTS-1, "bot");
}

void MGTarget::Tick()
{
	// nothing to tick
}