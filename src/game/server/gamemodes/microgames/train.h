// WarioWare mod by Headshotnoby

#ifndef _MICROGAME_TRAIN_H
#define _MICROGAME_TRAIN_H

#include "../microgame.h"

/*
	Avoid the train.
*/

class MGTrain : public Microgame
{
public:
	MGTrain(CGameContext* pGameServer, CGameControllerWarioWare* pController);
	~MGTrain() {}

	void Start();
	void End();
	void Tick();
	int OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon);
	void OnCharacterDamage(int Victim, int Killer, int Dmg, int Weapon) {}
	bool onChat(int client, const char *msg) {return false;}
};

#endif // _MICROGAME_HITENEMY_H