// WarioWare mod by Headshotnoby

#ifndef _MICROGAME_GRENADEJUMP_H
#define _MICROGAME_GRENADEJUMP_H

#include "../microgame.h"

/*
	Do a grenade jump to win.
*/

class MGGrenadeJump : public Microgame
{
public:
	MGGrenadeJump(CGameContext* pGameServer, CGameControllerWarioWare* pController);
	~MGGrenadeJump() {}

	void Start();
	void End();
	void Tick();
	int OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon) {return 0;}
	void OnCharacterDamage(int Victim, int Killer, int Dmg, int Weapon) {}
	bool onChat(int client, const char *msg) {return false;}
};

#endif // _MICROGAME_GRENADEJUMP_H