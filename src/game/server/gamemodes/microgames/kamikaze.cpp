// WarioWare mod by Headshotnoby

#include <engine/shared/config.h>
#include "kamikaze.h"

MGKamikaze::MGKamikaze(CGameContext* pGameServer, CGameControllerWarioWare* pController) : Microgame(pGameServer, pController)
{
	m_microgameName = "kamikaze";
	m_boss = false;
}

void MGKamikaze::Start()
{
	// pick a random ingame player
	do m_Victim = rand() % (MAX_CLIENTS-1);
	while (not GameServer()->m_apPlayers[m_Victim] or GameServer()->m_apPlayers[m_Victim]->GetTeam() == TEAM_SPECTATORS);

	// change their skin to a bomb
	CPlayer *pKamikaze = GameServer()->m_apPlayers[m_Victim];
	str_copy(pKamikaze->m_TeeInfos.m_SkinName, "bomb", sizeof(pKamikaze->m_TeeInfos.m_SkinName));
	pKamikaze->m_TeeInfos.m_UseCustomColor = 0;

	// count online players and send broadcast
	int online = 0;
	for (int i=0; i<MAX_CLIENTS-1; i++)
	{
		if (not GameServer()->m_apPlayers[i] or GameServer()->m_apPlayers[i]->GetTeam() == TEAM_SPECTATORS) continue;
		online++;

		CPlayer *pPlayer = GameServer()->m_apPlayers[i];
		pPlayer->SetInfoLock(true); // prevent them from changing skins

		if (i != m_Victim)
		{
			if (str_comp(pPlayer->m_TeeInfos.m_SkinName, "bomb") == 0) // remove fake
				str_copy(pPlayer->m_TeeInfos.m_SkinName, "default", sizeof(pPlayer->m_TeeInfos.m_SkinName));

			Controller()->g_Complete[i] = true;
			GameServer()->SendBroadcast("Avoid the kamikaze!", i);
		}
	}

	char aBuf[128];
	m_Amount = (online <= 6) ? 1 : 2; // exploding 2 players with not enough people online is a challenge.
	str_format(aBuf, sizeof(aBuf), "Explode %d %s!", m_Amount, (m_Amount > 1) ? "players" : "player");
	GameServer()->SendBroadcast(aBuf, m_Victim);

	// ddnet 9 gimmick, play map music
	Controller()->setPlayerTimers(g_Config.m_WwSndMgKamikaze_Offset, g_Config.m_WwSndMgKamikaze_Length);
}

void MGKamikaze::End()
{
	for (int i=0; i<MAX_CLIENTS-1; i++)
	{
		CPlayer *Player = GameServer()->m_apPlayers[i];
		if (Player) // revert skin
		{
			Player->SetInfoLock(false);
			str_copy(Player->m_TeeInfos.m_SkinName, Player->original_skin, sizeof(Player->m_TeeInfos.m_SkinName));
			Player->m_TeeInfos.m_UseCustomColor = Player->original_color;
		}
	}
}

void MGKamikaze::Tick()
{
	float timeLeft = Controller()->getTimeLength() - Controller()->getTimer(); // time in ms

	CCharacter *Char = GameServer()->GetPlayerChar(m_Victim);
	if (timeLeft < 600 and timeLeft > 200 and Char) // explode
	{
		int hits = GameServer()->CreateExplosion(Char->m_Pos, m_Victim, WEAPON_WORLD, false, -1, -1)-1;
		GameServer()->CreateSound(Char->m_Pos, SOUND_GRENADE_EXPLODE);

		if (hits >= m_Amount) Controller()->g_Complete[m_Victim] = true; // win
	}
}

void MGKamikaze::OnCharacterDamage(int Victim, int Killer, int Dmg, int Weapon)
{
	if (Weapon != WEAPON_WORLD) return;
	Controller()->killAndLoseMicroGame(Victim, Killer, Weapon);
}

int MGKamikaze::OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon)
{
	Controller()->g_Complete[pVictim->GetPlayer()->GetCID()] = false;
	return 0;
}
