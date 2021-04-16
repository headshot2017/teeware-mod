// WarioWare mod by Headshotnoby

#ifndef _MICROGAME_SIMON_H
#define _MICROGAME_SIMON_H

#include "../microgame.h"

/*
	If Simon says it, do it.
	If someone says it, don't.
*/

class MGSimon : public Microgame
{
public:
	MGSimon(CGameContext* pGameServer, CGameControllerWarioWare* pController);
	~MGSimon() {}

	void Start();
	void End();
	void Tick();

private:
	int m_Someone; // if "Someone says" then don't do it
	int m_SimonMode; // look up, look down, jump
	int m_SimonNegative; // do, don't
};

#endif // _MICROGAME_SIMON_H