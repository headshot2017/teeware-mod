// WarioWare mod by Headshotnoby

#ifndef _MICROGAME_PARACHUTE_H
#define _MICROGAME_PARACHUTE_H

#include "../microgame.h"

/*
	Land on the platform to win.
*/

class MGParachute : public Microgame
{
public:
	MGParachute(CGameContext* pGameServer, CGameControllerWarioWare* pController);
	~MGParachute() {}

	void Start();
	void End();
	void Tick();
	int OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon) {return 0;}
	void OnCharacterDamage(int Victim, int Killer, int Dmg, int Weapon) {}
	bool onChat(int client, const char *msg) {return false;}
};

#endif // _MICROGAME_PARACHUTE_H