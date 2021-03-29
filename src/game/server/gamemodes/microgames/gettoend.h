// WarioWare mod by Headshotnoby

#ifndef _MICROGAME_GETTOEND_H
#define _MICROGAME_GETTOEND_H

#include "../microgame.h"

/*
	Avoid the obstacles and get to the goal.
*/

class MGGetToEnd : public Microgame
{
public:
	MGGetToEnd(CGameContext* pGameServer, CGameControllerWarioWare* pController);
	~MGGetToEnd() {}

	void Start();
	void End();
	void Tick();
	int OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon) {return 0;}
	void OnCharacterDamage(int Victim, int Killer, int Dmg, int Weapon) {}
	bool onChat(int client, const char *msg) {return false;}
};

#endif // _MICROGAME_GETTOEND_H