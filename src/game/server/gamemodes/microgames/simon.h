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
	int OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon) {return 0;}
	void OnCharacterDamage(int Victim, int Killer, int Dmg, int Weapon) {}
	bool onChat(int client, const char *msg) {return false;}

private:
	int m_Someone; // if "Someone says" then don't do it
	int m_SimonMode; // look up, look down, jump
};

#endif // _MICROGAME_SIMON_H