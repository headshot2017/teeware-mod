// WarioWare mod by Headshotnoby

#include <engine/shared/config.h>
#include <game/server/faketee.h>
#include "ninjasurvival.h"

MGNinjaSurvival::MGNinjaSurvival(CGameContext* pGameServer, CGameControllerWarioWare* pController) : Microgame(pGameServer, pController)
{
	m_microgameName = "ninjasurvival";
	m_boss = true;
}

void MGNinjaSurvival::Start()
{
	m_Moved = false;
	m_PathFound = false;
	m_startTick = Server()->Tick();

	for (int i=0; i<MAX_CLIENTS-1; i++)
	{
		CPlayer *Player = GameServer()->m_apPlayers[i];
		CCharacter *Char = (Player) ? Player->GetCharacter() : 0;
		if (not Char) continue;

		Player->SetInfoLock(true); // prevent skin change
		if (str_comp_nocase(Server()->ClientName(i), "ninja") == 0) // remove fake
			Server()->SetClientName(i, "fake");

		Controller()->g_Complete[i] = (GameServer()->GetPlayerChar(i));
	}

	GameServer()->SendBroadcast("Survive the ninja!", -1);
	Controller()->setPlayerTimers(g_Config.m_WwSndMgSurvive_Offset, g_Config.m_WwSndMgSurvive_Length);
}

void MGNinjaSurvival::End()
{
	Server()->SetClientName(MAX_CLIENTS-1, "bot");
	GameServer()->m_apPlayers[MAX_CLIENTS-1]->SetTeam(TEAM_SPECTATORS, false); // move to spec

	// reset player healths
	for (int i=0; i<MAX_CLIENTS-1; i++)
	{
		CPlayer *Player = GameServer()->m_apPlayers[i];
		CCharacter *Char = (Player) ? Player->GetCharacter() : 0;

		if (not Player) continue;
		Player->SetInfoLock(false);
		str_copy(Player->m_TeeInfos.m_SkinName, Player->original_skin, sizeof(Player->m_TeeInfos.m_SkinName));

		if (not Char) continue;
		Char->SetHealth(10);
	}
}

void MGNinjaSurvival::Tick()
{
	if (Server()->Tick() - m_startTick > 225 and not m_Moved)
	{
		Server()->SetClientName(MAX_CLIENTS-1, "Ninja");
		GameServer()->m_apPlayers[MAX_CLIENTS-1]->SetTeam(0, false); // move to game
		m_Moved = true;

		m_Target = -1;
		m_SwitchTargetTick = 0;
		m_FireTick = 20;
	}
	else if (m_Moved) // bot tick
	{
		CCharacter *Bot = GameServer()->GetPlayerChar(MAX_CLIENTS-1);
		if (Bot)
		{
			Bot->GiveNinja();

			float timeLeft = Controller()->getTimeLength() - Controller()->getTimer();
			if (timeLeft < 3000)
			{
				Bot->SetEmoteType(EMOTE_SURPRISE);
				Bot->SetEmoteStop(Server()->Tick() + Server()->TickSpeed());
			}
		}
	}
}

void MGNinjaSurvival::OnCharacterDamage(int Victim, int Killer, int Dmg, int Weapon)
{
	if (Killer == MAX_CLIENTS-1)
	{
		CCharacter *pVictim = GameServer()->GetPlayerChar(Victim);
		CCharacter *pKiller = GameServer()->GetPlayerChar(Killer);
		if (not pVictim or not pKiller) return;

		pVictim->SetHealth(pVictim->GetHealth() - Dmg);

		if(pVictim->GetHealth() <= 0)
		{
			Controller()->killAndLoseMicroGame(Victim, Killer, Weapon);

			// set attacker's face to happy (taunt!)
			pKiller->SetEmoteType(EMOTE_HAPPY);
			pKiller->SetEmoteStop(Server()->Tick() + Server()->TickSpeed());
			return;
		}

		if (Dmg > 2)
			GameServer()->CreateSound(pVictim->m_Pos, SOUND_PLAYER_PAIN_LONG);
		else
			GameServer()->CreateSound(pVictim->m_Pos, SOUND_PLAYER_PAIN_SHORT);
	}
}

void MGNinjaSurvival::OnBotInput(CNetObj_PlayerInput* Input)
{
	CCharacter *Bot = GameServer()->GetPlayerChar(MAX_CLIENTS-1);
	CCharacter *Target = GameServer()->GetPlayerChar(m_Target);

	if (not Bot) return;

	if (m_SwitchTargetTick <= 0 or not Target)
	{
		int loops = 0;
		m_PathFound = false;
		do
		{
			m_Target = rand() % (MAX_CLIENTS-1);
			loops++;
		}
		while (loops < 300 and not (Target = GameServer()->GetPlayerChar(m_Target)));

		if (loops == 300) // everyone died
		{
			for (int i=0; i<MAX_CLIENTS-1; i++)
			{
				if (not GameServer()->m_apPlayers[i] or GameServer()->m_apPlayers[i]->GetTeam() == TEAM_SPECTATORS)
					continue;

				GameServer()->m_apPlayers[i]->Respawn();
			}
			Controller()->nextWarioState(); // force the microgame to end
			return;
		}

		m_SwitchTargetTick = 175; // 3.5 secs
	}

	m_SwitchTargetTick--;

	bool Wall = (GameServer()->Collision()->IntersectLine(Bot->m_Pos, Target->m_Pos, NULL, NULL, false) != 0);
	int dirX = sign(Target->m_Pos.x - Bot->m_Pos.x);
	//int dirY = sign(Target->m_Pos.y - Bot->m_Pos.y);

	if (not Wall)
	{
		m_PathFound = false;
		Input->m_Hook = 0;
		Input->m_Direction = dirX;
		Input->m_TargetX = Target->m_Pos.x - Bot->m_Pos.x;
		Input->m_TargetY = Target->m_Pos.y - Bot->m_Pos.y;

		m_FireTick = (Wall) ? 25 : m_FireTick-1;
		if (m_FireTick <= 0)
		{
			Input->m_Fire = 1;
			m_FireTick = 40;
		}
		else
			Input->m_Fire = 0;
	}
	else
	{
		if (not m_PathFound)
		{
			FakeTee::pathFind(GameServer(), 0, Target->m_Pos);
			m_PathFound = true;
		}
		else
		{
			FakeTee::pathFindMove(GameServer(), 0, Input);
		}
	}
}
