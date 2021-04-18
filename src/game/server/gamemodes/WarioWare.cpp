// WarioWare mod by Headshotnoby

#include <engine/server.h>
#include <engine/shared/config.h>
#include <game/mapitems.h>
#include <game/server/entities/character.h>
#include <game/server/player.h>
#include <game/server/gamecontext.h>
#include "WarioWare.h"
#include "gamemode.h"

// microgames
#include "microgames/kamikaze.h"
#include "microgames/math.h"
#include "microgames/hitenemy.h"
#include "microgames/dontmove.h"
#include "microgames/grenadejump.h"
#include "microgames/simon.h"
#include "microgames/gettoend.h"
#include "microgames/blockfight.h"
#include "microgames/train.h"
#include "microgames/superjump.h"
#include "microgames/parachute.h"
#include "microgames/flood.h"
#include "microgames/target.h"
#include "microgames/tilecolors.h"
#include "microgames/bombrain.h"
#include "microgames/cow.h"
#include "microgames/luckydoor.h"
#include "microgames/reachendnade1.h"
#include "microgames/reachendnade2.h"
#include "microgames/piggyback.h"

// bosses
#include "microgames/passball.h"
#include "microgames/ninjasurvival.h"


CGameControllerWarioWare::CGameControllerWarioWare(class CGameContext *pGameServer) :
		IGameController(pGameServer), m_Teams(pGameServer)
{
	m_pGameType = "TeeWare";
	InitTeleporter();
	
	srand(time(0));
	m_microgame = -1;
	for (int i=0; i<5; i++) m_last_microgame[i] = -1;

	// add all microgames here
	m_microgames.push_back(new MGKamikaze(pGameServer, this));
	m_microgames.push_back(new MGMath(pGameServer, this));
	m_microgames.push_back(new MGHitEnemy(pGameServer, this));
	m_microgames.push_back(new MGDontMove(pGameServer, this));
	m_microgames.push_back(new MGGrenadeJump(pGameServer, this));
	m_microgames.push_back(new MGSimon(pGameServer, this));
	m_microgames.push_back(new MGGetToEnd(pGameServer, this));
	m_microgames.push_back(new MGBlockFight(pGameServer, this));
	m_microgames.push_back(new MGTrain(pGameServer, this));
	m_microgames.push_back(new MGSuperJump(pGameServer, this));
	m_microgames.push_back(new MGParachute(pGameServer, this));
	m_microgames.push_back(new MGFlood(pGameServer, this));
	m_microgames.push_back(new MGTarget(pGameServer, this));
	m_microgames.push_back(new MGTileColors(pGameServer, this));
	m_microgames.push_back(new MGBombRain(pGameServer, this));
	m_microgames.push_back(new MGHitCow(pGameServer, this));
	m_microgames.push_back(new MGPassBall(pGameServer, this));
	m_microgames.push_back(new MGLuckyDoor(pGameServer, this));
	m_microgames.push_back(new MGNinjaSurvival(pGameServer, this));
	m_microgames.push_back(new MGReachEndNade1(pGameServer, this));
	m_microgames.push_back(new MGReachEndNade2(pGameServer, this));
	m_microgames.push_back(new MGPiggyback(pGameServer, this));
}

CGameControllerWarioWare::~CGameControllerWarioWare()
{
	while (not m_microgames.empty())
	{
		delete m_microgames.back();
		m_microgames.pop_back();
	}
	m_microgames.clear();
}

void CGameControllerWarioWare::StartRound()
{
	int online = 0;
	for (int i=0; i<MAX_CLIENTS-1; i++)
	{
		if (not GameServer()->m_apPlayers[i]) continue;
		GameServer()->m_apPlayers[i]->m_Score = 0;
		if (GameServer()->m_apPlayers[i]->IsVoluntarySpectator()) continue;
		else if (GameServer()->m_apPlayers[i]->GetTeam() == TEAM_SPECTATORS)
			GameServer()->m_apPlayers[i]->SetTeam(0, false);

		++online;
	}

	m_state = (online < 2) ? WW_WAITING : WW_INGAME;
	m_warioState = WW_JINGLING;
	m_round = 0;
	m_speedUp = false;
	finalWinners.clear();
	
	if (m_state == WW_WAITING)
	{
		GameServer()->SendBroadcast("Waiting for players...", -1);
		int songs1[2] = {g_Config.m_WwSndWaiting1_Offset, g_Config.m_WwSndWaiting2_Offset};
		int songs2[2] = {g_Config.m_WwSndWaiting1_Length, g_Config.m_WwSndWaiting2_Length};
		int ind = rand() % 2;
		setPlayerTimers(songs1[ind], songs2[ind]);
	}
	else
	{
		GameServer()->SendBroadcast("", -1);
		setPlayerTimers(g_Config.m_WwSndJingle_Offset, g_Config.m_WwSndJingle_Length);
	}
}

void CGameControllerWarioWare::setPlayerTimers(float offset, float length)
{
	for (int i=0; i<MAX_CLIENTS; i++)
	{
		if (not GameServer()->m_apPlayers[i]) continue;
		CCharacter *Char = GameServer()->m_apPlayers[i]->GetCharacter();

		if (Char)
			Char->setTimer(offset-60);
		else
			GameServer()->m_apPlayers[i]->SetSpawnTimer(offset-60);
	}
	
	setTimer(offset-60);
	warioTimeLength = offset+length;
}

void CGameControllerWarioWare::teleportPlayer(int client, int tele_id)
{
	CCharacter *Char = GameServer()->GetPlayerChar(client);

	if (not Char) return;

	int Num = m_TeleOuts[tele_id-1].size();
	if (!Num)
	{
		printf("not num???\n");
		return;
	}
	vec2 outPos = m_TeleOuts[tele_id-1][(!Num)?Num:rand() % Num];
	Char->m_Pos = Char->Core()->m_Pos = Char->m_PrevPos = outPos;

	Char->Core()->m_HookedPlayer = -1;
	Char->Core()->m_HookState = HOOK_RETRACTED;
	Char->Core()->m_TriggeredEvents |= COREEVENT_HOOK_RETRACT;
	GameServer()->m_World.ReleaseHooked(client);
	Char->Core()->m_Vel = vec2(0,0);
	Char->Core()->m_HookPos = Char->Core()->m_Pos;
}

void CGameControllerWarioWare::teleportPlayerToSpawn(int client)
{
	CPlayer *Player = GameServer()->m_apPlayers[client];
	CCharacter *Char = (Player) ? Player->GetCharacter() : 0;
	if (not Char) return;

	vec2 SpawnPos;
	if(CanSpawn(Player->GetTeam(), &SpawnPos))
	{
		Char->Core()->m_HookedPlayer = -1;
		Char->Core()->m_HookState = HOOK_RETRACTED;
		Char->Core()->m_TriggeredEvents |= COREEVENT_HOOK_RETRACT;
		Char->m_Pos = Char->Core()->m_Pos = Char->m_PrevPos = SpawnPos;
		GameServer()->m_World.ReleaseHooked(client);
		Char->Core()->m_Vel = vec2(0,0);
		Char->Core()->m_HookPos = Char->Core()->m_Pos;
	}
}

void CGameControllerWarioWare::nextWarioState()
{
	char aBuf[128];
	
	switch(m_warioState)
	{
		case WW_JINGLING:
			rollMicroGame();
			m_warioState = WW_MICROGAME;
			break;
		
		case WW_MICROGAME: // microgame finished
			m_round++;

			onMicroGameEnd();
			if (not m_speedUp)
				setPlayerTimers(g_Config.m_WwSndLose_Offset, g_Config.m_WwSndLose_Length);
			else
				setPlayerTimers(g_Config.m_WwSndLoseFast_Offset, g_Config.m_WwSndLoseFast_Length);

			for (int i=0; i<MAX_CLIENTS-1; i++)
			{
				if (not GameServer()->m_apPlayers[i] or GameServer()->m_apPlayers[i]->IsVoluntarySpectator()) continue;

				GameServer()->SendBroadcast((g_Complete[i]) ? "You win!" : "You failed...", i);
				CCharacter *Char = GameServer()->GetPlayerChar(i);

				if (g_Complete[i])
				{
					GameServer()->m_apPlayers[i]->m_Score += (m_round == g_Config.m_WwMaxRounds) ? 5 : 1;
					if (Char) Char->setTimer((not m_speedUp) ? g_Config.m_WwSndWin_Offset : g_Config.m_WwSndWinFast_Offset);
					else GameServer()->m_apPlayers[i]->SetSpawnTimer((not m_speedUp) ? g_Config.m_WwSndWin_Offset : g_Config.m_WwSndWinFast_Offset);
				}
			}
			m_warioState = WW_WINLOSE;
			break;
		
		case WW_WINLOSE: // finished showing win/lose text
			if (m_round+1 == g_Config.m_WwMaxRounds/2) // speedup
			{
				GameServer()->SendBroadcast("Speed up!", -1);
				setPlayerTimers(g_Config.m_WwSndSpeedUp_Offset, g_Config.m_WwSndSpeedUp_Length);
				m_warioState = WW_SPEEDUP;
				m_speedUp = true;
				break;
			}
			else if (m_round+1 == g_Config.m_WwMaxRounds) // boss
			{
				GameServer()->SendBroadcast("   Boss time!\nWorth 5 points", -1);
				setPlayerTimers(g_Config.m_WwSndBoss_Offset, g_Config.m_WwSndBoss_Length);
				m_warioState = WW_BOSSWARN;
				m_speedUp = false;
				break;
			}
			else if (m_round+1 > g_Config.m_WwMaxRounds)
			{
				doGameOver();
				break;
			}
			// go to jingle
		case WW_SPEEDUP:
		case WW_BOSSWARN:
			str_format(aBuf, sizeof(aBuf), "Round %d", m_round+1);
			GameServer()->SendBroadcast(aBuf, -1); // clear win/lose message

			if (not m_speedUp)
				setPlayerTimers(g_Config.m_WwSndJingle_Offset, g_Config.m_WwSndJingle_Length);
			else
				setPlayerTimers(g_Config.m_WwSndJingleFast_Offset, g_Config.m_WwSndJingleFast_Length);

			m_warioState = WW_JINGLING;
			break;
		
		case WW_GAMEOVER:
			for (int i=0; i<MAX_CLIENTS; i++)
				if (GameServer()->m_apPlayers[i]) GameServer()->m_apPlayers[i]->m_Score = 0;

			IGameController::StartRound();
			GameServer()->SendBroadcast("", -1);
			break;
	}
}

void CGameControllerWarioWare::onMicroGameEnd()
{
	m_microgames[m_microgame]->End();
	for (int i=0; i<MAX_CLIENTS-1; i++)
	{
		CPlayer *Player = GameServer()->m_apPlayers[i];
		CCharacter *Char = GameServer()->GetPlayerChar(i);

		if (Player and Player->IsOut())
		{
			Player->SetTeam(0, false);
			if (not Char) Player->Respawn();
		}

		if (not Char) continue;

		Char->SetHookOthers(true);
		Char->SetHitOthers(true);
		Char->SetCollideOthers(true);
		Char->SetSuperJump(false);
	}
}

void CGameControllerWarioWare::rollMicroGame()
{
	if (g_Config.m_WwForceMicrogame < 0)
	{
		if (m_round+1 < g_Config.m_WwMaxRounds)
		{
			do m_microgame = rand() % m_microgames.size();
			while (((m_microgame == m_last_microgame[0] or 
					 m_microgame == m_last_microgame[1] or 
					 m_microgame == m_last_microgame[2] or 
					 m_microgame == m_last_microgame[3] or 
					 m_microgame == m_last_microgame[4]) and m_microgames.size() > 5) or m_microgames[m_microgame]->m_boss);
		}
		else
		{
			do m_microgame = rand() % m_microgames.size();
			while (not m_microgames[m_microgame]->m_boss);
		}

		for (int i=4; i>=1; i--)
			m_last_microgame[i] = m_last_microgame[i-1];
		m_last_microgame[0] = m_microgame;
	}
	else
		m_microgame = g_Config.m_WwForceMicrogame;

	int online = 0;
	for (int i=0; i<MAX_CLIENTS-1; i++)
	{
		if (not GameServer()->m_apPlayers[i] or GameServer()->m_apPlayers[i]->IsVoluntarySpectator()) continue;
		g_Complete[i] = false;
		++online;
	}

	char aBuf[128];
	str_format(aBuf, sizeof(aBuf), "round %d: '%s'", m_round+1, m_microgames[m_microgame]->m_microgameName);
	GameServer()->Console()->Print(IConsole::OUTPUT_LEVEL_DEBUG, "TeeWare", aBuf);
	m_microgames[m_microgame]->Start();
}

void CGameControllerWarioWare::doGameOver()
{
	int highest = 0;
	std::vector<CPlayer*> winners;

	setPlayerTimers(g_Config.m_WwSndFinalLose_Offset, g_Config.m_WwSndFinalLose_Length);

	for (int i=0; i<MAX_CLIENTS-1; i++)
	{
		if (GameServer()->m_apPlayers[i] and not GameServer()->m_apPlayers[i]->IsVoluntarySpectator())
		{
			CPlayer *Player = GameServer()->m_apPlayers[i];
			if (Player->m_Score > highest)
				highest = Player->m_Score;
		}
	}

	// again but get players with the same score.
	for (int i=0; i<MAX_CLIENTS-1; i++)
	{
		if (GameServer()->m_apPlayers[i] and not GameServer()->m_apPlayers[i]->IsVoluntarySpectator())
		{
			CPlayer *Player = GameServer()->m_apPlayers[i];
			if (Player->m_Score == highest)
				finalWinners.push_back(i);
		}
	}

	char aBuf[128];

	std::string winStr;
	for (unsigned i=0; i<finalWinners.size(); i++)
	{
		char name[128];

		str_format(name, sizeof(name), "%s'%s'%s",
			(finalWinners.size() > 1 and i == finalWinners.size()-1) ? "and " : "",
			Server()->ClientName(finalWinners[i]),
			(finalWinners.size() > 1 and i < finalWinners.size()-2) ? ", " : " "
		);

		winStr += name;

		CPlayer *Player = GameServer()->m_apPlayers[finalWinners[i]];
		if (Player->GetCharacter())
			Player->GetCharacter()->setTimer(g_Config.m_WwSndFinalWin_Offset);
		else
			Player->SetSpawnTimer(g_Config.m_WwSndFinalWin_Offset);
	}

	str_format(aBuf, sizeof(aBuf), "%s%s!", winStr.c_str(), (finalWinners.size() > 1) ? "win" : "wins");

	GameServer()->SendBroadcast(aBuf, -1);
	m_warioState = WW_GAMEOVER;
}

bool CGameControllerWarioWare::onChat(int client, const char *msg)
{
	if (m_state == WW_WAITING or m_warioState != WW_MICROGAME or not GameServer()->m_apPlayers[client]->GetCharacter()) return false;

	return m_microgames[m_microgame]->onChat(client, msg);
}

void CGameControllerWarioWare::winMicroGame(int client)
{
	CCharacter *Char = GameServer()->GetPlayerChar(client);
	if (g_Complete[client] or not Char) return;
	
	g_Complete[client] = true;
	GameServer()->CreateSound(Char->m_Pos, SOUND_HIT);
	GameServer()->CreateSoundGlobal(SOUND_CTF_CAPTURE, client);
	GameServer()->CreateDeath(Char->m_Pos, client);
}

void CGameControllerWarioWare::killPlayer(int client, int killer, int weapon)
{
	CCharacter *Char = GameServer()->GetPlayerChar(client);
	if (not Char) return;
	
	float timeLeft = getTimeLength() - getTimer();
	Char->Die(killer, weapon, timeLeft/1000.0f);
}

void CGameControllerWarioWare::killAndLoseMicroGame(int client, int killer, int weapon)
{
	killPlayer(client, killer, weapon);
	g_Complete[client] = false;
}

int CGameControllerWarioWare::OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon)
{
	if (isInGame() and inMicroGame())
	{
		return m_microgames[m_microgame]->OnCharacterDeath(pVictim, pKiller, Weapon);
	}

	return 0;
}

void CGameControllerWarioWare::OnCharacterDamage(int Victim, int Killer, int Dmg, int Weapon)
{
	if (isInGame() and inMicroGame())
		m_microgames[m_microgame]->OnCharacterDamage(Victim, Killer, Dmg, Weapon);
}

void CGameControllerWarioWare::Tick()
{
	IGameController::Tick();
	if (getTimer() < 500) return;
	else if (getTimer() == 500) StartRound();
	
	int online = 0;
	for (int i=0; i<MAX_CLIENTS-1; i++)
	{
		if (not Server()->ClientIngame(i) or GameServer()->m_apPlayers[i]->IsVoluntarySpectator()) continue;
		CPlayer *Player = GameServer()->m_apPlayers[i];
		CCharacter *Char = Player->GetCharacter();
		++online;
		if (not Char) continue;
		
		// locally (for each player)
		switch(m_state)
		{
			case WW_WAITING:
				{
					float first_song = g_Config.m_WwSndWaiting1_Offset + g_Config.m_WwSndWaiting1_Length;
					float offsettime = (Char->getTimer() < first_song+10) ? g_Config.m_WwSndWaiting1_Offset : g_Config.m_WwSndWaiting2_Offset;
					float lengthtime = (Char->getTimer() < first_song+10) ? g_Config.m_WwSndWaiting1_Length : g_Config.m_WwSndWaiting2_Length;
					
					if (Char->getTimer() >= offsettime + lengthtime)
					{
						Char->setTimer(offsettime-5);
					}
				}
				break;
		}
	}

	// globally (for everyone)
	switch(m_state)
	{
		case WW_WAITING:
			if (online < 2)
			{
				if (GameServer()->Server()->Tick() % 150 == 0) GameServer()->SendBroadcast("Waiting for players...", -1);
			}
			else
			{
				if (not m_Warmup) DoWarmup(5);
			}
			break;
		
		case WW_INGAME:
			if (online < 2)
			{
				GameServer()->SendBroadcast("Waiting for players...", -1);
				int songs1[2] = {g_Config.m_WwSndWaiting1_Offset, g_Config.m_WwSndWaiting2_Offset};
				int songs2[2] = {g_Config.m_WwSndWaiting1_Length, g_Config.m_WwSndWaiting2_Length};
				int ind = rand() % 2;
				setPlayerTimers(songs1[ind], songs2[ind]);

				if (inMicroGame())
					m_microgames[m_microgame]->End();

				m_state = WW_WAITING;
				IGameController::StartRound();
				StartRound();
			}
			else
			{
				if (inMicroGame())
				{
					m_microgames[m_microgame]->Tick();
				}
				
				if (getTimer() > warioTimeLength)
					nextWarioState();
			}
			break;
	}
}

void CGameControllerWarioWare::InitTeleporter()
{
	if (!GameServer()->Collision()->Layers()->TeleLayer())
		return;
	int Width = GameServer()->Collision()->Layers()->TeleLayer()->m_Width;
	int Height = GameServer()->Collision()->Layers()->TeleLayer()->m_Height;

	for (int i = 0; i < Width * Height; i++)
	{
		if (GameServer()->Collision()->TeleLayer()[i].m_Number > 0)
		{
			if (GameServer()->Collision()->TeleLayer()[i].m_Type
					== TILE_TELEOUT)
			{
				m_TeleOuts[GameServer()->Collision()->TeleLayer()[i].m_Number
						- 1].push_back(
						vec2(i % Width * 32 + 16, i / Width * 32 + 16));
			}
			else if (GameServer()->Collision()->TeleLayer()[i].m_Type
					== TILE_TELECHECKOUT)
			{
				m_TeleCheckOuts[GameServer()->Collision()->TeleLayer()[i].m_Number
						- 1].push_back(
						vec2(i % Width * 32 + 16, i / Width * 32 + 16));
			}
		}
	}
}
