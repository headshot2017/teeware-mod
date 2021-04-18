// WarioWare mod by Headshotnoby

#ifndef _MICROGAME_TARGET_H
#define _MICROGAME_TARGET_H

#include "../microgame.h"

/*
	Hit the target with your laser to win.
*/

class MGTarget : public Microgame
{
private:
	int m_UseWeapon;
public:
	MGTarget(CGameContext* pGameServer, CGameControllerWarioWare* pController);
	~MGTarget() {}

	void Start();
	void End();
	void Tick();
	void OnCharacterDamage(int Victim, int Killer, int Dmg, int Weapon);
};

#endif // _MICROGAME_TARGET_H