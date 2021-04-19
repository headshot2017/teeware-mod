// WarioWare mod by Headshotnoby

#include <engine/shared/config.h>
#include "piggyback.h"

MGPiggyback::MGPiggyback(CGameContext* pGameServer, CGameControllerWarioWare* pController) : Microgame(pGameServer, pController)
{
	m_microgameName = "piggyback";
	m_boss = false;
}

void MGPiggyback::Start()
{
	// count online players
	std::vector<int> online;
	std::map<int, bool> alreadyPinky, alreadyTwintri;

	m_twintris.clear();
	m_pinkys.clear();
	m_piggybackingWho.clear();

	for (int i=0; i<MAX_CLIENTS-1; i++)
	{
		CPlayer *Player = GameServer()->m_apPlayers[i];
		CCharacter *Char = GameServer()->GetPlayerChar(i);
		if (not Char) continue;

		Player->SetInfoLock(true);
		Char->SetHitOthers(false);
		Char->SetHookOthers(false);
		Char->SetCollideOthers(false);

		online.push_back(i);
	}

	// check if enough players for piggyback kills
	m_PiggybackKillAmnt = (online.size()-1 < 3) ? online.size()-1 : 3;

	// count twintris
	unsigned int twintris = online.size() / 3 + 1;

	// count pinkys
	unsigned int pinkys = online.size() - twintris;

	// pick twintris randomly
	while (alreadyTwintri.size() < twintris)
	{
		int player;
		do player = online[rand() % online.size()];
		while (alreadyTwintri.count(player)); // pick a player that hasn't been teleported yet
		alreadyTwintri[player] = true; // don't teleport them again
		m_twintris.push_back(player);

		CPlayer *Player = GameServer()->m_apPlayers[player];
		CCharacter *Char = GameServer()->GetPlayerChar(player);
		Char->m_ForcedTuneZone = 4; // no, it won't crash here. see above on the online vector; this is guaranteed an online player

		str_copy(Player->m_TeeInfos.m_SkinName, "twintri", sizeof(Player->m_TeeInfos.m_SkinName));
		Player->m_TeeInfos.m_UseCustomColor = 0;

		GameServer()->SendBroadcast("Don't get piggybacked!\n          (Go right)          ", player); // centered text hack
		Controller()->teleportPlayer(player, 22);
	}

	// pick pinkys randomly
	while (alreadyPinky.size() < pinkys)
	{
		int player;
		do player = online[rand() % online.size()];
		while (alreadyTwintri.count(player) or alreadyPinky.count(player)); // pick a player that hasn't been teleported yet
		alreadyPinky[player] = true; // don't teleport them again
		m_pinkys.push_back(player);

		CPlayer *Player = GameServer()->m_apPlayers[player];
		str_copy(Player->m_TeeInfos.m_SkinName, "pinky", sizeof(Player->m_TeeInfos.m_SkinName));
		Player->m_TeeInfos.m_UseCustomColor = 0;

		GameServer()->SendBroadcast("Piggyback the green tees!", player);
		Controller()->teleportPlayer(player, 20);
	}

	Controller()->setPlayerTimers(g_Config.m_WwSndMgPiggyback_Offset, g_Config.m_WwSndMgPiggyback_Length);
}

void MGPiggyback::End()
{
	for (int i=0; i<MAX_CLIENTS-1; i++)
	{
		CPlayer *Player = GameServer()->m_apPlayers[i];
		CCharacter *Char = GameServer()->GetPlayerChar(i);

		if (Player) // revert skin
		{
			Player->SetInfoLock(false);
			str_copy(Player->m_TeeInfos.m_SkinName, Player->original_skin, sizeof(Player->m_TeeInfos.m_SkinName));
			Player->m_TeeInfos.m_UseCustomColor = Player->original_color;
		}

		if (Char)
		{
			Controller()->teleportPlayerToSpawn(i);
			Char->m_ForcedTuneZone = -1;
		}
	}
}

void MGPiggyback::Tick()
{
	int piggybackCount[MAX_CLIENTS] = {0}; // amount of pinkys currently piggybacking twintris

	int alive = 0;
	for (unsigned i=0; i<m_twintris.size(); i++)
	{
		CCharacter *Char = GameServer()->GetPlayerChar(m_twintris[i]);
		if (Char)
			alive++;
	}

	if (not alive) // no twintris alive
	{
		for (unsigned i=0; i<m_pinkys.size(); i++)
		{
			Controller()->winMicroGame(m_pinkys[i]);
		}
	}
	else
	{
		for (unsigned i=0; i<m_pinkys.size(); i++)
		{
			int client = m_pinkys[i];
			bool piggyback = (m_piggybackingWho.count(client));
			CCharacter *Char = GameServer()->GetPlayerChar(client);
			if (not Char)
			{
				if (piggyback) m_piggybackingWho.erase(client);
				continue;
			}

			if (piggyback) 
			{
				int twintri = m_piggybackingWho[client];
				CCharacter *cTwintri = GameServer()->GetPlayerChar(twintri);
				if (not cTwintri) // twintri disappeared or died
				{
					m_piggybackingWho.erase(client);
					continue;
				}

				piggybackCount[twintri]++;
				if (piggybackCount[twintri] >= m_PiggybackKillAmnt) // TOO HEAVY die
				{
					float timeLeft = Controller()->getTimeLength() - Controller()->getTimer();
					cTwintri->Die(twintri, WEAPON_WORLD, timeLeft/1000.f);
					m_piggybackingWho.erase(client);
					continue;
				}

				Char->Core()->m_Pos = Char->m_Pos = (cTwintri->m_Pos - vec2(16, 24)); // put piggybacker behind twintri
				Char->Core()->m_Vel = vec2(0,0); // no speed
			}
			else
			{
				CCharacter *aEnts[MAX_CLIENTS];
				float Radius = Char->m_ProximityRadius * 1.5f;
				int Num = GameServer()->m_World.FindEntities(Char->m_Pos, Radius, (CEntity**)aEnts, MAX_CLIENTS, CGameWorld::ENTTYPE_CHARACTER);

				for (int ii = 0; ii < Num; ii++)
				{
					if (aEnts[ii] == Char)
						continue;
					
					// check so we are sufficiently close
					if (distance(aEnts[ii]->m_Pos, Char->m_Pos) > Radius)
						continue;

					int twintri = aEnts[ii]->GetPlayer()->GetCID();
					bool isTwintri = false;
					for (unsigned iii=0; iii<m_twintris.size(); iii++)
					{
						if (m_twintris[iii] == twintri)
						{
							isTwintri = true;
							break;
						}
					}
					if (not isTwintri)
						continue;

					Controller()->winMicroGame(client);
					m_piggybackingWho[client] = twintri;
				}
			}
		}
	}
}

bool MGPiggyback::OnWinMicrogame(int client, int winTile)
{
	for (unsigned i=0; i<m_pinkys.size(); i++)
	{
		if (m_pinkys[i] == client) // don't let pinkys win
			return false;
	}

	if (winTile == TILE_WARIOWARE_REACHEND_NADE1_WIN) // kill all pinkys
	{
		for (unsigned i=0; i<m_pinkys.size(); i++)
		{
			CCharacter *Char = GameServer()->GetPlayerChar(m_pinkys[i]);
			if (not Char) continue;

			float timeLeft = Controller()->getTimeLength() - Controller()->getTimer();
			Char->Die(m_pinkys[i], WEAPON_WORLD, timeLeft/1000.f);
		}

		// all twintris win
		for (unsigned i=0; i<m_twintris.size(); i++)
		{
			Controller()->winMicroGame(m_twintris[i]);
		}
		
		return true;
	}
	return false;
}

int MGPiggyback::OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon)
{
	//int victim = pVictim->GetPlayer()->GetCID();
	int alive = 0;
	for (unsigned i=0; i<m_twintris.size(); i++)
	{
		CCharacter *Char = GameServer()->GetPlayerChar(m_twintris[i]);
		if (Char and Char != pVictim)
			alive++;
	}

	if (not alive) // all twintris died
	{
		for (unsigned i=0; i<m_pinkys.size(); i++)
		{
			Controller()->winMicroGame(m_pinkys[i]);
		}
	}

	return 0;
}
