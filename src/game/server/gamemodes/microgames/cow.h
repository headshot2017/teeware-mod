// WarioWare mod by Headshotnoby

#ifndef _MICROGAME_COW_H
#define _MICROGAME_COW_H

#include "../microgame.h"

/*
	Milk the cow to win! (moo)
*/

class MGHitCow : public Microgame
{
public:
	MGHitCow(CGameContext* pGameServer, CGameControllerWarioWare* pController);
	~MGHitCow() {}

	void Start();
	void End();
	void Tick();
	int OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon) {return 0;}
	void OnCharacterDamage(int Victim, int Killer, int Dmg, int Weapon);
	bool onChat(int client, const char *msg) {return false;}
};

#endif // _MICROGAME_COW_H