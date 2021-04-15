// WarioWare mod by Headshotnoby

#ifndef _MICROGAME_PASSBALL_H
#define _MICROGAME_PASSBALL_H

#include "../microgame.h"
#include <game/server/gameworld.h>
#include <game/server/entity.h>
#include <game/server/entities/projectile.h>

/*
	The team with least balls on their side wins.
*/


class MGPassBall : public Microgame
{
public:
	MGPassBall(CGameContext* pGameServer, CGameControllerWarioWare* pController);
	~MGPassBall() {}

	void Start();
	void End();
	void Tick();

	void OnCharacterDamage(int Victim, int Killer, int Dmg, int Weapon);

	void pushBall(CProjectile *pProj); // push a ball stuck in the middle to the nearest side

private:
	bool m_GameOver; // scores calculated and game is over
	std::vector<vec2> m_BallSpawns;
	std::vector<int> m_LeftTeamPlayers;
	std::vector<int> m_RightTeamPlayers;
	vec2 m_SeparatorPos; // horizontal line that separates left and right teams for score calculation
	vec2 m_SeparatorLeft;
	vec2 m_SeparatorRight;
};

#endif // _MICROGAME_PASSBALL_H