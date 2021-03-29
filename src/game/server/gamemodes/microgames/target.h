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
	int OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon) {return 0;}
	void OnCharacterDamage(int Victim, int Killer, int Dmg, int Weapon) {}
	bool onChat(int client, const char *msg) {return false;}
};

#endif // _MICROGAME_TARGET_H