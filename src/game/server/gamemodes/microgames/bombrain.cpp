// WarioWare mod by Headshotnoby

#include <engine/shared/config.h>
#include <game/server/gameworld.h>
#include <game/server/entity.h>
#include <game/server/entities/projectile.h>
#include "bombrain.h"


MGBombRain::MGBombRain(CGameContext* pGameServer, CGameControllerWarioWare* pController) : Microgame(pGameServer, pController)
{
	m_microgameName = "bombrain";
	m_boss = false;

	// load the map's bomb rain entities
	int Width = GameServer()->Collision()->GetWidth();
	int Height = GameServer()->Collision()->GetHeight();

	for (int i = 0; i < Width * Height; i++)
	{
		switch (GameServer()->Collision()->GameLayer()[i].m_Index)
		{
			case TILE_WARIOWARE_BOMBRIGHT:
				m_BombRainEntities.push_back(BombEntity(vec2(i % Width * 32 + 16, i / Width * 32 + 16), vec2(1, 0)));
				break;

			case TILE_WARIOWARE_BOMBLEFT:
				m_BombRainEntities.push_back(BombEntity(vec2(i % Width * 32 + 16, i / Width * 32 + 16), vec2(-1, 0)));
				break;

			case TILE_WARIOWARE_BOMBDOWN:
				m_BombRainEntities.push_back(BombEntity(vec2(i % Width * 32 + 16, i / Width * 32 + 16), vec2(0, 0)));
				break;
		}
	}

	char buf[128];
	str_format(buf, sizeof(buf), "%d total bomb rain entities", m_BombRainEntities.size());
	GameServer()->Console()->Print(IConsole::OUTPUT_LEVEL_DEBUG, "TeeWare::bombrain", buf);
}

void MGBombRain::Start()
{
	for (int i=0; i<MAX_CLIENTS; i++)
		Controller()->g_Complete[i] = true;

	m_startTick = Server()->Tick();
	GameServer()->SendBroadcast("Avoid the bombs!", -1);
	Controller()->setPlayerTimers(g_Config.m_WwSndMgBombRain_Offset, g_Config.m_WwSndMgBombRain_Length);

	// set to a slower speed
	GameServer()->Tuning()->m_GrenadeCurvature.Set((int)(7.f*100.0f));
	GameServer()->Tuning()->m_GrenadeSpeed.Set((int)(600.f*100.0f));
	GameServer()->Tuning()->m_GrenadeLifetime.Set((int)(10*100.0f));
	GameServer()->TuningList()[1].m_GrenadeCurvature.Set((int)(7.f*100.0f));
	GameServer()->TuningList()[1].m_GrenadeSpeed.Set((int)(600.f*100.0f));
	GameServer()->TuningList()[1].m_GrenadeLifetime.Set((int)(10*100.0f));

	// sync tunes
	GameServer()->SendTuningParams(-1);
	GameServer()->SendTuningParams(-1, 1);
}

void MGBombRain::End()
{
	// reset to default tuning
	GameServer()->Tuning()->m_GrenadeCurvature.Set((int)(7.f*100.0f));
	GameServer()->Tuning()->m_GrenadeSpeed.Set((int)(1000.f*100.0f));
	GameServer()->Tuning()->m_GrenadeLifetime.Set((int)(2*100.0f));
	GameServer()->TuningList()[1].m_GrenadeCurvature.Set((int)(7.f*100.0f));
	GameServer()->TuningList()[1].m_GrenadeSpeed.Set((int)(1000.f*100.0f));
	GameServer()->TuningList()[1].m_GrenadeLifetime.Set((int)(2*100.0f));

	// sync tunes
	GameServer()->SendTuningParams(-1);
	GameServer()->SendTuningParams(-1, 1);
}

void MGBombRain::Tick()
{
	float timeLeft = Controller()->getTimeLength() - Controller()->getTimer();

	if (timeLeft < 7900 and timeLeft > 650) // start dropping nukes
	{
		if ((Server()->Tick() + m_startTick) % 10 == 0) // drop one
		{
			// pick a random pos
			BombEntity *ent = &m_BombRainEntities[rand() % m_BombRainEntities.size()];

			// taken from character.cpp
			int Lifetime = (int)(Server()->TickSpeed()*GameServer()->Tuning()->m_GrenadeLifetime);

			CProjectile *pProj = new CProjectile
					(
					&GameServer()->m_World,
					WEAPON_GRENADE,//Type
					-1,//Owner
					ent->m_Pos,//Pos
					ent->m_StartSpeed,//Dir
					Lifetime,//Span
					0,//Freeze
					true,//Explosive
					0,//Force
					SOUND_GRENADE_EXPLODE,//SoundImpact
					WEAPON_GRENADE//Weapon
					);

			pProj = pProj; // GTFO UNUSED VARIABLE WARNING
		}
	}
}

void MGBombRain::OnCharacterDamage(int Victim, int Killer, int Dmg, int Weapon)
{
	if (Killer == -1 and Weapon == WEAPON_GRENADE) // unlucky guy hit by bomb
	{
		Controller()->killAndLoseMicroGame(Victim, -1, Weapon);
	}
}