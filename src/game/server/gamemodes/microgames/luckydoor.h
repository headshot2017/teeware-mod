// WarioWare mod by Headshotnoby

#ifndef _MICROGAME_LUCKYDOOR_H
#define _MICROGAME_LUCKYDOOR_H

#include "../microgame.h"

/*
	One of four doors is rigged as the correct one.
	Pick the correct one to win!
*/


class MGLuckyDoor : public Microgame
{
public:
	MGLuckyDoor(CGameContext* pGameServer, CGameControllerWarioWare* pController);
	~MGLuckyDoor() {}

	void Start();
	void End();
	void Tick();

private:
	std::vector< std::pair<int, CTeleTile*> > m_DoorTeles; // pair: int (original tele ID number), CTeleTile* (the actual tele tile)
};

#endif // _MICROGAME_LUCKYDOOR_H