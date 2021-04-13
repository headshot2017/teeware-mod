// WarioWare mod by Headshotnoby

#ifndef _MICROGAME_REACHENDNADE1_H
#define _MICROGAME_REACHENDNADE1_H

#include "../microgame.h"

/*
	Reach the end and avoid the bouncing grenades!
	You start at the far end of the map.
*/


class MGReachEndNade1 : public Microgame
{
public:
	MGReachEndNade1(CGameContext* pGameServer, CGameControllerWarioWare* pController);
	~MGReachEndNade1() {}

	void Start();
	void End();
	void Tick();

	void OnCharacterDamage(int Victim, int Killer, int Dmg, int Weapon);
	bool OnWinMicrogame(int client, int winTile);

private:
	std::vector<vec2> m_NadeSpawnEntities;
};

#endif // _MICROGAME_REACHENDNADE1_H