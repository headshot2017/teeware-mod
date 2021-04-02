// WarioWare mod by Headshotnoby

#ifndef _MICROGAME_FLOOD_H
#define _MICROGAME_FLOOD_H

#include "../microgame.h"

/*
	Get on a platform and avoid the rising acid.
*/

class MGFlood : public Microgame
{
public:
	MGFlood(CGameContext* pGameServer, CGameControllerWarioWare* pController);
	~MGFlood() {}

	void Start();
	void End();
	void Tick();
	int OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon);
};

#endif // _MICROGAME_HITENEMY_H