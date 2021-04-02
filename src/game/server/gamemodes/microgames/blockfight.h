// WarioWare mod by Headshotnoby

#ifndef _MICROGAME_BLOCKFIGHT_H
#define _MICROGAME_BLOCKFIGHT_H

#include "../microgame.h"

/*
	Fight and stay alive.
*/

class MGBlockFight : public Microgame
{
public:
	MGBlockFight(CGameContext* pGameServer, CGameControllerWarioWare* pController);
	~MGBlockFight() {}

	void Start();
	void End();
	void Tick();
	int OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon);
};

#endif // _MICROGAME_BLOCKFIGHT_H