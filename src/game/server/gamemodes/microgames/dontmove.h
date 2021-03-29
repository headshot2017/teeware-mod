// WarioWare mod by Headshotnoby

#ifndef _MICROGAME_DONTMOVE_H
#define _MICROGAME_DONTMOVE_H

#include "../microgame.h"

/*
	Don't move, or don't stop moving.
	(Firing your weapon/hook also counts as moving!)
*/

class MGDontMove : public Microgame
{
public:
	MGDontMove(CGameContext* pGameServer, CGameControllerWarioWare* pController);
	~MGDontMove() {}

	void Start();
	void End();
	void Tick();
	int OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon) {return 0;}
	void OnCharacterDamage(int Victim, int Killer, int Dmg, int Weapon) {}
	bool onChat(int client, const char *msg) {return false;}

private:
	int m_Mode; // whether it's "don't move" or "don't stop moving"
};

#endif // _MICROGAME_DONTMOVE_H