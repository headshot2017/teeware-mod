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
};

#endif // _MICROGAME_GRENADEJUMP_H