// WarioWare mod by Headshotnoby

#include <engine/shared/config.h>
#include "target.h"

const int weapons[4] = { WEAPON_GUN, WEAPON_GRENADE, WEAPON_SHOTGUN, WEAPON_RIFLE };
const char *weaponNames[4] = { "PISTOL", "GRENADES", "SHOTGUN", "LASER" };

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
		
		for (int j=1; j<4; j++)
			Char->GiveWeapon(weapons[j], 3);
		Char->SetWeapon(weapons[rand() % 4]);
		
		Controller()->teleportPlayer(i, 6);
	}

	// teleport the bot player (the target)
	m_UseWeapon = rand() % 4;
	int bot_tele = 7;
	int Num = Controller()->m_TeleOuts[bot_tele-1].size();
	Server()->SetClientName(MAX_CLIENTS-1, "HIT ME");
	GameServer()->m_apPlayers[MAX_CLIENTS-1]->SetTeam(0, false); // move to game
	GameServer()->m_apPlayers[MAX_CLIENTS-1]->ForceSpawn(Controller()->m_TeleOuts[bot_tele-1][(!Num)?Num:rand() % Num]);

	char aBuf[128];
	str_format(aBuf, sizeof(aBuf), "Hit the target with your %s!", weaponNames[m_UseWeapon]);
	GameServer()->SendBroadcast(aBuf, -1);
	Controller()->setPlayerTimers(g_Config.m_WwSndMgHitTheTarget_Offset, g_Config.m_WwSndMgHitTheTarget_Length);
}

void MGTarget::End()
{
	for (int i=0; i<MAX_CLIENTS-1; i++)
	{
		CCharacter *Char = GameServer()->GetPlayerChar(i);
		if (not Char) continue;

		for (int j=1; j<4; j++)
			Char->SetWeaponGot(weapons[j], false);

		Char->SetWeapon(WEAPON_HAMMER);

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

void MGTarget::OnCharacterDamage(int Victim, int Killer, int Dmg, int Weapon)
{
	if (Victim == MAX_CLIENTS-1)
	{
		if (Weapon == weapons[m_UseWeapon]) {
			Controller()->winMicroGame(Killer);
		} else {
			if (!Controller()->g_Complete[Killer]) {
				Controller()->killAndLoseMicroGame(Killer);
				GameServer()->SendChatTarget(Killer, "You used the wrong weapon...");
			}
		}
	}
}