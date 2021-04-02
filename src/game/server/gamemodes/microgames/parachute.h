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
};

#endif // _MICROGAME_PARACHUTE_H