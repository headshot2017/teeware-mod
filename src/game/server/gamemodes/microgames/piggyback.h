// WarioWare mod by Headshotnoby

#ifndef _MICROGAME_PIGGYBACK_H
#define _MICROGAME_PIGGYBACK_H

#include "../microgame.h"

/*
	pinkys must piggyback the twintris
	twintris must escape
*/


class MGPiggyback : public Microgame
{
public:
	MGPiggyback(CGameContext* pGameServer, CGameControllerWarioWare* pController);
	~MGPiggyback() {}

	void Start();
	void End();
	void Tick();

	bool OnWinMicrogame(int client, int winTile);
	int OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon);

private:
	int m_PiggybackKillAmnt; // amount of pinkys that must piggyback a twintri to kill them. in case of less than 3 players
	std::vector<int> m_pinkys; // pinky skin players (piggybackers)
	std::vector<int> m_twintris; // twintri skin players (must escape pinkys)
	std::map<int, int> m_piggybackingWho; // which pinky players are piggybacking twintri players
};

#endif // _MICROGAME_PIGGYBACK_H