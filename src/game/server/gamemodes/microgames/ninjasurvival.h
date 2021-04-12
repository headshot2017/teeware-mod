// WarioWare mod by Headshotnoby

#ifndef _MICROGAME_NINJASURVIVAL_H
#define _MICROGAME_NINJASURVIVAL_H

#include "../microgame.h"

/*
	The bot joins the game as a ninja and attacks everyone.
	Survive!
*/


class MGNinjaSurvival : public Microgame
{
public:
	MGNinjaSurvival(CGameContext* pGameServer, CGameControllerWarioWare* pController);
	~MGNinjaSurvival() {}

	void Start();
	void End();
	void Tick();

	void OnCharacterDamage(int Victim, int Killer, int Dmg, int Weapon);
	void OnBotInput(CNetObj_PlayerInput* Input);

private:
	bool m_Moved; // there is a delay before the bot is moved ingame, to sync with music. this bool makes sure the bot isn't moved twice.
	int m_startTick;

	// AI stuff
	int m_FireTick; // ticks to wait before firing. give the player time to react!
	int m_Target; // current player target
	int m_SwitchTargetTick; // ticks before switching to another target
	bool m_PathFound; // if executed pathfind function
};

#endif // _MICROGAME_NINJASURVIVAL_H