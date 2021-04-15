// WarioWare mod by Headshotnoby

#include <engine/shared/config.h>
#include "passball.h"

MGPassBall::MGPassBall(CGameContext* pGameServer, CGameControllerWarioWare* pController) : Microgame(pGameServer, pController)
{
	m_microgameName = "passball";
	m_boss = true;

	// load the map's ball spawn and team separator entities
	int Width = GameServer()->Collision()->GetWidth();
	int Height = GameServer()->Collision()->GetHeight();

	std::vector<vec2> teamseps;
	float sepLeft = 0, sepRight = 0;

	for (int i = 0; i < Width * Height; i++)
	{
		switch (GameServer()->Collision()->GameLayer()[i].m_Index)
		{
			case TILE_WARIOWARE_BALLSPAWN:
				m_BallSpawns.push_back(vec2(i % Width * 32 + 16, i / Width * 32 + 16));
				break;

			case TILE_WARIOWARE_BALLTEAMSEP:
				teamseps.push_back(vec2(i % Width * 32 + 16, i / Width * 32 + 16));
				sepLeft = (sepLeft > 0) ? min(sepLeft, teamseps.back().x) : teamseps.back().x;
				sepRight = (sepRight > 0) ? max(sepRight, teamseps.back().x) : teamseps.back().x;
				break;
		}
	}

	for (unsigned int i=0; i<teamseps.size(); i++)
	{
		m_SeparatorPos.x += teamseps[i].x / teamseps.size();
		m_SeparatorPos.y += teamseps[i].y / teamseps.size();
	}
	m_SeparatorLeft = vec2(sepLeft, m_SeparatorPos.y);
	m_SeparatorRight = vec2(sepRight, m_SeparatorPos.y);

	char buf[128];
	str_format(buf, sizeof(buf), "%d total ball spawn entities", m_BallSpawns.size());
	GameServer()->Console()->Print(IConsole::OUTPUT_LEVEL_DEBUG, "TeeWare::passball", buf);
}

void MGPassBall::Start()
{
	std::map<int, bool> alreadyInTeam;
	std::vector<int> online;

	// get online players
	for (int i=0; i<MAX_CLIENTS; i++)
	{
		CCharacter *Char = GameServer()->GetPlayerChar(i);
		if (not Char) continue;

		online.push_back(i);
	}

	// teleport players randomly to a team (balanced of course)
	int tele = 11 + (rand() % 2); // start randomly with left/right team

	while (alreadyInTeam.size() < online.size())
	{
		int player;
		do player = online[rand() % online.size()];
		while (alreadyInTeam.count(player)); // pick a player that hasn't been teleported yet
		alreadyInTeam[player] = true; // don't teleport them again

		Controller()->teleportPlayer(player, tele);
		if (tele == 11) // left team
		{
			m_LeftTeamPlayers.push_back(player);
			tele++;
		}
		else // right team
		{
			m_RightTeamPlayers.push_back(player);
			tele--;
		}
	}

	// create the balls
	for (unsigned i=0; i<m_BallSpawns.size(); i++)
	{
		// taken from character.cpp
		int Lifetime = (int)(Server()->TickSpeed()*GameServer()->Tuning()->m_GrenadeLifetime);

		CProjectile *pProj = new CProjectile
				(
				&GameServer()->m_World,
				WEAPON_GRENADE,//Type
				-1,//Owner
				m_BallSpawns[i],//Pos
				vec2(0,0),//Dir
				Lifetime,//Span
				0,//Freeze
				true,//Explosive
				0,//Force
				SOUND_GRENADE_EXPLODE,//SoundImpact
				WEAPON_GRENADE//Weapon
				);

		pProj->SetFootMode(1);
	}

	m_GameOver = false;

	GameServer()->SendBroadcast("Pass the balls to the other team!\nThe team with least balls wins!", -1);
	Controller()->setPlayerTimers(g_Config.m_WwSndMgPassBall_Offset, g_Config.m_WwSndMgPassBall_Length);
}

void MGPassBall::End()
{
	for (int i=0; i<MAX_CLIENTS; i++)
	{
		CCharacter *Char = GameServer()->GetPlayerChar(i);
		if (not Char) continue;

		Controller()->teleportPlayerToSpawn(i);
	}

	// delete stray balls again just in case...
	CProjectile *p = (CProjectile *)GameServer()->m_World.FindFirst(CGameWorld::ENTTYPE_PROJECTILE);
	for(; p; p = (CProjectile *)p->TypeNext())
	{
		if (not p->GetFootMode())
			continue;

		GameServer()->m_World.DestroyEntity(p);
	}

	m_LeftTeamPlayers.clear();
	m_RightTeamPlayers.clear();
}

void MGPassBall::Tick()
{
	float timeLeft = Controller()->getTimeLength() - Controller()->getTimer();

	if (timeLeft < 2000 and not m_GameOver)
	{
		m_GameOver = true;
		int leftScore = 0;
		int rightScore = 0;

		// calculate score from the balls then delete them
		CProjectile *p = (CProjectile *)GameServer()->m_World.FindFirst(CGameWorld::ENTTYPE_PROJECTILE);
		for(; p; p = (CProjectile *)p->TypeNext())
		{
			if (not p->GetFootMode())
				continue;

			if (p->m_Pos.x < m_SeparatorPos.x)
				leftScore++;
			else
				rightScore++;

			GameServer()->m_World.DestroyEntity(p);
		}

		// calculate score from players holding balls
		for (unsigned l=0, r=0; l<m_LeftTeamPlayers.size() or r<m_RightTeamPlayers.size(); l++, r++)
		{
			CCharacter *CharL = (l<m_LeftTeamPlayers.size()) ? GameServer()->GetPlayerChar(m_LeftTeamPlayers[l]) : 0;
			CCharacter *CharR = (r<m_RightTeamPlayers.size()) ? GameServer()->GetPlayerChar(m_RightTeamPlayers[r]) : 0;

			if (CharL)
			{
				if (CharL->GetWeaponGot(WEAPON_GRENADE)) leftScore++;

				CharL->SetWeaponGot(WEAPON_GRENADE, false);
				if (CharL->GetActiveWeapon() == WEAPON_GRENADE and CharL->GetLastWeapon() != WEAPON_GRENADE)
					CharL->SetWeapon(CharL->GetLastWeapon());
			}
			if (CharR)
			{
				if (CharR->GetWeaponGot(WEAPON_GRENADE)) rightScore++;

				CharR->SetWeaponGot(WEAPON_GRENADE, false);
				if (CharR->GetActiveWeapon() == WEAPON_GRENADE and CharR->GetLastWeapon() != WEAPON_GRENADE)
					CharR->SetWeapon(CharR->GetLastWeapon());
			}
		}

		char abuf[128], abuf2[128];
		str_format(abuf, sizeof(abuf), "Scores: %d-%d", leftScore, rightScore);

		if (leftScore < rightScore)
		{
			str_format(abuf2, sizeof(abuf2), "Left team wins!");
			for (unsigned i=0; i<m_LeftTeamPlayers.size(); i++)
				Controller()->winMicroGame(m_LeftTeamPlayers[i]);
		}
		else if (rightScore < leftScore)
		{
			str_format(abuf2, sizeof(abuf2), "Right team wins!");
			for (unsigned i=0; i<m_RightTeamPlayers.size(); i++)
				Controller()->winMicroGame(m_RightTeamPlayers[i]);
		}
		else
			str_format(abuf2, sizeof(abuf2), "Draw!");

		GameServer()->SendChat(-1, CGameContext::CHAT_ALL, abuf);
		GameServer()->SendChat(-1, CGameContext::CHAT_ALL, abuf2);
	}
}

void MGPassBall::OnCharacterDamage(int Victim, int Killer, int Dmg, int Weapon)
{
	if (Weapon == WEAPON_HAMMER) // hit with hammer
	{
		CCharacter *cVictim = GameServer()->GetPlayerChar(Victim);
		CCharacter *cKiller = GameServer()->GetPlayerChar(Killer);
		if (not cVictim or not cKiller) return;

		// take their grenade. helpful against afks/trolls
		if (cVictim->GetWeaponGot(WEAPON_GRENADE) and not cKiller->GetWeaponGot(WEAPON_GRENADE))
		{
			cVictim->SetWeaponGot(WEAPON_GRENADE, false);
			if (cVictim->GetActiveWeapon() == WEAPON_GRENADE and cVictim->GetLastWeapon() != WEAPON_GRENADE)
				cVictim->SetWeapon(cVictim->GetLastWeapon());

			cKiller->GiveWeapon(WEAPON_GRENADE, -1);
			cKiller->SetWeapon(WEAPON_GRENADE);
			cKiller->SetActiveWeapon(WEAPON_GRENADE); // again just in case
		}
	}
}

void MGPassBall::pushBall(CProjectile* pProj)
{
	if (pProj->m_Pos.x >= m_SeparatorLeft.x and pProj->m_Pos.x <= m_SeparatorRight.x) // push ball stuck in the middle
	{
		int dir = sign(pProj->m_Pos.x - m_SeparatorPos.x);
		pProj->AddDirection(vec2(dir, 0));
	}
}