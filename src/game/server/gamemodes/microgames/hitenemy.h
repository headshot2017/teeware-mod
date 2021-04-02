// WarioWare mod by Headshotnoby

#ifndef _MICROGAME_HITENEMY_H
#define _MICROGAME_HITENEMY_H

#include "../microgame.h"

/*
	Hit a player with your hammer to win.
*/

class MGHitEnemy : public Microgame
{
public:
	MGHitEnemy(CGameContext* pGameServer, CGameControllerWarioWare* pController);
	~MGHitEnemy() {}

	void Start();
	void End();
	void Tick();
	int OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon);
	void OnCharacterDamage(int Victim, int Killer, int Dmg, int Weapon);
};

#endif // _MICROGAME_HITENEMY_H