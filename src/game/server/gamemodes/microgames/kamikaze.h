// WarioWare mod by Headshotnoby

#ifndef _MICROGAME_KAMIKAZE_H
#define _MICROGAME_KAMIKAZE_H

#include "../microgame.h"

/*
	Explosive player microgame.
	The kamikaze must blow up 1 or 2 players depending on online players.
	The rest of the players must avoid the kamikaze.
*/

class MGKamikaze : public Microgame
{
public:
	MGKamikaze(CGameContext* pGameServer, CGameControllerWarioWare* pController);
	~MGKamikaze() {}

	void Start();
	void End();
	void Tick();
	int OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon);
	void OnCharacterDamage(int Victim, int Killer, int Dmg, int Weapon);

private:
	int m_Victim; // randomly selected kamikaze
	int m_Amount; // kamikaze must blow up this amount of players
};

#endif // _MICROGAME_KAMIKAZE_H