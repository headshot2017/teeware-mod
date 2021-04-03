// WarioWare mod by Headshotnoby

#include <engine/shared/config.h>
#include "luckydoor.h"

MGLuckyDoor::MGLuckyDoor(CGameContext* pGameServer, CGameControllerWarioWare* pController) : Microgame(pGameServer, pController)
{
	m_microgameName = "luckydoor";
	m_boss = false;
}

void MGLuckyDoor::Start()
{
	int CorrectTele = rand() % 4 + 14; // first door tele ID starts at 14
	m_DoorTeles.clear();

	char abuf[128];
	str_format(abuf, sizeof(abuf), "correct tele: %d", CorrectTele);
	GameServer()->Console()->Print(IConsole::OUTPUT_LEVEL_DEBUG, "TeeWare::luckydoor", abuf);

	// rig teles 14-17
	int Width = GameServer()->Collision()->GetWidth();
	int Height = GameServer()->Collision()->GetHeight();

	for (int i=0; i<Width*Height; i++)
	{
		if (GameServer()->Collision()->TeleLayer()[i].m_Type == TILE_TELEINEVIL)
		{
			if (GameServer()->Collision()->TeleLayer()[i].m_Number == CorrectTele)
			{
				// rig as correct
				m_DoorTeles.push_back(std::pair<int, CTeleTile*>(GameServer()->Collision()->TeleLayer()[i].m_Number, &GameServer()->Collision()->TeleLayer()[i]));

				GameServer()->Collision()->TeleLayer()[i].m_Number = 18;
			}
			else if (GameServer()->Collision()->TeleLayer()[i].m_Number >= 14 and GameServer()->Collision()->TeleLayer()[i].m_Number <= 17)
			{
				// rig as wrong
				m_DoorTeles.push_back(std::pair<int, CTeleTile*>(GameServer()->Collision()->TeleLayer()[i].m_Number, &GameServer()->Collision()->TeleLayer()[i]));

				GameServer()->Collision()->TeleLayer()[i].m_Number = 19;
			}
		}
	}

	for (int i=0; i<MAX_CLIENTS; i++)
	{
		CCharacter *Char = GameServer()->GetPlayerChar(i);
		if (not Char) continue;

		Char->SetHookOthers(false);
		Char->SetHitOthers(false);
		Char->SetCollideOthers(false);
		
		Controller()->teleportPlayer(i, 13);
	}

	GameServer()->SendBroadcast("Enter a door!", -1);
	Controller()->setPlayerTimers(g_Config.m_WwSndMgLuckyDoor_Offset, g_Config.m_WwSndMgLuckyDoor_Length);
}

void MGLuckyDoor::End()
{
	// unrig the affected teles
	for (unsigned i=0; i<m_DoorTeles.size(); i++)
	{
		m_DoorTeles[i].second->m_Number = m_DoorTeles[i].first;
	}

	for (int i=0; i<MAX_CLIENTS; i++)
	{
		CCharacter *Char = GameServer()->GetPlayerChar(i);
		if (not Char) continue;
		
		Controller()->teleportPlayerToSpawn(i);
	}
}

void MGLuckyDoor::Tick()
{
	
}