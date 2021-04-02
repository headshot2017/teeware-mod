// WarioWare mod by Headshotnoby

#ifndef _MICROGAME_TARGET_H
#define _MICROGAME_TARGET_H

#include "../microgame.h"

/*
	Hit the target with your laser to win.
*/

class MGTarget : public Microgame
{
public:
	MGTarget(CGameContext* pGameServer, CGameControllerWarioWare* pController);
	~MGTarget() {}

	void Start();
	void End();
	void Tick();
};

#endif // _MICROGAME_TARGET_H