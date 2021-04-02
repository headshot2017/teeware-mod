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
};

#endif // _MICROGAME_SUPERJUMP_H