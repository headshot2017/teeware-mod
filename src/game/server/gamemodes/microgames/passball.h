// WarioWare mod by Headshotnoby

#ifndef _MICROGAME_PASSBALL_H
#define _MICROGAME_PASSBALL_H

#include "../microgame.h"

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

private:
	bool m_GameOver; // scores calculated and game is over
	std::vector<vec2> m_BallSpawns;
	std::vector<int> m_LeftTeamPlayers;
	std::vector<int> m_RightTeamPlayers;
	vec2 m_SeparatorPos; // horizontal line that separates left and right teams for score calculation
};

#endif // _MICROGAME_PASSBALL_H