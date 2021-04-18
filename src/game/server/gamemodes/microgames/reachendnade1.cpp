// WarioWare mod by Headshotnoby

#include <engine/shared/config.h>
#include <game/server/gameworld.h>
#include <game/server/entity.h>
#include <game/server/entities/projectile.h>
#include "reachendnade1.h"

MGReachEndNade1::MGReachEndNade1(CGameContext* pGameServer, CGameControllerWarioWare* pController) : Microgame(pGameServer, pController)
{
	m_microgameName = "reachendnade1";
	m_boss = false;

	// load the map's nade spawn entities
	int Width = GameServer()->Collision()->GetWidth();
	int Height = GameServer()->Collision()->GetHeight();

	for (int i = 0; i < Width * Height; i++)
	{
		if (GameServer()->Collision()->GameLayer()[i].m_Index == TILE_WARIOWARE_REACHEND_NADESPAWN)
		{
			m_NadeSpawnEntities.push_back(vec2(i % Width * 32 + 16, i / Width * 32 + 16));
		}
	}

	char buf[128];
	str_format(buf, sizeof(buf), "%d total nade spawn entities", m_NadeSpawnEntities.size());
	GameServer()->Console()->Print(IConsole::OUTPUT_LEVEL_DEBUG, "TeeWare::reachendnade1", buf);
}

void MGReachEndNade1::Start()
{
	for (int i=0; i<MAX_CLIENTS-1; i++)
	{
		CCharacter *Char = GameServer()->GetPlayerChar(i);
		if (not Char) continue;

		Char->SetHitOthers(false);
		Char->SetHookOthers(false);
		Char->SetCollideOthers(false);

		Controller()->teleportPlayer(i, 20);
	}

	Controller()->setPlayerTimers(g_Config.m_WwSndMgReachEndNade1_Offset, g_Config.m_WwSndMgReachEndNade1_Length);
	GameServer()->SendBroadcast("Reach the end!", -1);
}

void MGReachEndNade1::End()
{
	for (int i=0; i<MAX_CLIENTS-1; i++)
	{
		CCharacter *Char = GameServer()->GetPlayerChar(i);
		if (not Char) continue;

		Controller()->teleportPlayerToSpawn(i);
	}

	// delete stray nades
	CProjectile *p = (CProjectile *)GameServer()->m_World.FindFirst(CGameWorld::ENTTYPE_PROJECTILE);
	for(; p; p = (CProjectile *)p->TypeNext())
	{
		if (not p->GetFootMode())
			continue;

		GameServer()->m_World.DestroyEntity(p);
	}
}

void MGReachEndNade1::Tick()
{
	if (Server()->Tick() % 75 == 0) // spawn a grenade
	{
		// taken from character.cpp
		int Lifetime = 60 * Server()->TickSpeed();

		CProjectile *pProj = new CProjectile
				(
				&GameServer()->m_World,
				WEAPON_GRENADE,//Type
				-1,//Owner
				m_NadeSpawnEntities[rand() % m_NadeSpawnEntities.size()],//Pos
				vec2(-0.4,0),//Dir
				Lifetime,//Span
				0,//Freeze
				true,//Explosive
				0,//Force
				SOUND_GRENADE_EXPLODE,//SoundImpact
				WEAPON_GRENADE//Weapon
				);

		pProj->SetFootMode(2);
	}
}

void MGReachEndNade1::OnCharacterDamage(int Victim, int Killer, int Dmg, int Weapon)
{
	if (Killer == -1 and Weapon == WEAPON_GRENADE)
	{
		Controller()->killPlayer(Victim, -1, Weapon);
	}
}

bool MGReachEndNade1::OnWinMicrogame(int client, int winTile)
{
	return (winTile == TILE_WARIOWARE_REACHEND_NADE1_WIN);
}