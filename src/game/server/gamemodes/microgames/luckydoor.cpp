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
	int CorrectTeles[2] = {rand() % 4 + 14, rand() % 4 + 14}; // first door tele ID starts at 14
	while (CorrectTeles[1] == CorrectTeles[0])
		CorrectTeles[1] = rand() % 4 + 14;

	m_DoorTeles.clear();

	char abuf[128];
	str_format(abuf, sizeof(abuf), "correct teles: %d %d", CorrectTeles[0], CorrectTeles[1]);
	GameServer()->Console()->Print(IConsole::OUTPUT_LEVEL_DEBUG, "TeeWare::luckydoor", abuf);

	// rig teles 14-17
	int Width = GameServer()->Collision()->GetWidth();
	int Height = GameServer()->Collision()->GetHeight();

	for (int i=0; i<Width*Height; i++)
	{
		if (GameServer()->Collision()->TeleLayer()[i].m_Type == TILE_TELEINEVIL)
		{
			if (GameServer()->Collision()->TeleLayer()[i].m_Number == CorrectTeles[0] or
				GameServer()->Collision()->TeleLayer()[i].m_Number == CorrectTeles[1])
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

		//Char->SetHookOthers(false);
		//Char->SetHitOthers(false);
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