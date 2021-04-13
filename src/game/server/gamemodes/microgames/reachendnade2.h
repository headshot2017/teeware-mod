// WarioWare mod by Headshotnoby

#ifndef _MICROGAME_REACHENDNADE2_H
#define _MICROGAME_REACHENDNADE2_H

#include "../microgame.h"

/*
	Reach the end and avoid the bouncing grenades!
	You start right at the grenade spawners.
*/


class MGReachEndNade2 : public Microgame
{
public:
	MGReachEndNade2(CGameContext* pGameServer, CGameControllerWarioWare* pController);
	~MGReachEndNade2() {}

	void Start();
	void End();
	void Tick();

	void OnCharacterDamage(int Victim, int Killer, int Dmg, int Weapon);
	bool OnWinMicrogame(int client, int winTile);

private:
	std::vector<vec2> m_NadeSpawnEntities;
};

#endif // _MICROGAME_REACHENDNADE2_H