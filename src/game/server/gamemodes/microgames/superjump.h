// WarioWare mod by Headshotnoby

#ifndef _MICROGAME_SUPERJUMP_H
#define _MICROGAME_SUPERJUMP_H

#include "../microgame.h"

/*
	Touch the sky with infinite jump.
*/

class MGSuperJump : public Microgame
{
public:
	MGSuperJump(CGameContext* pGameServer, CGameControllerWarioWare* pController);
	~MGSuperJump() {}

	void Start();
	void End();
	void Tick();
	int OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon) {return 0;}
	void OnCharacterDamage(int Victim, int Killer, int Dmg, int Weapon) {}
	bool onChat(int client, const char *msg) {return false;}
};

#endif // _MICROGAME_SUPERJUMP_H