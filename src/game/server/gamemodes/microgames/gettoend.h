// WarioWare mod by Headshotnoby

#ifndef _MICROGAME_GETTOEND_H
#define _MICROGAME_GETTOEND_H

#include "../microgame.h"

/*
	Avoid the obstacles and get to the goal.
*/

class MGGetToEnd : public Microgame
{
public:
	MGGetToEnd(CGameContext* pGameServer, CGameControllerWarioWare* pController);
	~MGGetToEnd() {}

	void Start();
	void End();
	void Tick();
};

#endif // _MICROGAME_GETTOEND_H