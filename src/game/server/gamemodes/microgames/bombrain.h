// WarioWare mod by Headshotnoby

#ifndef _MICROGAME_BOMBRAIN_H
#define _MICROGAME_BOMBRAIN_H

#include "../microgame.h"

/*
	It's raining bombs! Avoid them to survive!
*/


struct BombEntity // holds info about entities 182 to 184 on the map. see mapitems.h
{
	BombEntity(vec2 pos, vec2 startspd) {m_Pos = pos; m_StartSpeed = startspd;}
	vec2 m_Pos;
	vec2 m_StartSpeed;
};


class MGBombRain : public Microgame
{
public:
	MGBombRain(CGameContext* pGameServer, CGameControllerWarioWare* pController);
	~MGBombRain() {}

	void Start();
	void End();
	void Tick();
	void OnCharacterDamage(int Victim, int Killer, int Dmg, int Weapon);

private:
	int m_startTick;
	
	std::vector<BombEntity> m_BombRainEntities;
};

#endif // _MICROGAME_BOMBRAIN_H