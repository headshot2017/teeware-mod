// WarioWare mod by Headshotnoby

#ifndef WARIOWARE_H
#define WARIOWARE_H
#include <game/server/gamecontroller.h>
#include <game/server/teams.h>

#include <vector>
#include <map>

class Microgame;
#include "microgame.h"

enum // m_state
{
	WW_WAITING,
	WW_INGAME
};

enum
{
	WW_GAMESTART,
	WW_JINGLING,
	WW_MICROGAME,
	WW_WINLOSE,
	WW_SPEEDUP,
	WW_BOSSWARN,
	WW_GAMEOVER
};

class CGameControllerWarioWare: public IGameController
{
public:

	CGameControllerWarioWare(class CGameContext *pGameServer);
	~CGameControllerWarioWare();

	CGameTeams m_Teams;

	std::map<int, std::vector<vec2> > m_TeleOuts;
	std::map<int, std::vector<vec2> > m_TeleCheckOuts;

	void InitTeleporter();
	virtual void Tick();
	virtual int OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon);
	virtual void OnCharacterDamage(int Victim, int Killer, int Dmg, int Weapon);
	void StartRound();
	
	// get values
	int getState() { return m_state; }
	int getWarioState() { return m_warioState; }
	Microgame* getMicroGame() { return m_microgames[m_microgame]; }
	int getRound() { return m_round; }
	float getTimeLength() const { return warioTimeLength; }
	bool isInGame() { return m_state == WW_INGAME; }
	bool inMicroGame() { return m_warioState == WW_MICROGAME; }
	
	// warioware
	void rollMicroGame();
	void winMicroGame(int client);
	void killAndLoseMicroGame(int client, int killer = -1, int weapon = -1);
	void killPlayer(int client, int killer = -1, int weapon = -1);
	void onMicroGameEnd();
	void nextWarioState();
	void doGameOver();
	bool onChat(int client, const char *msg);
	
	// player control
	void setPlayerTimers(float offset, float length);
	void teleportPlayer(int client, int tele_id);
	void teleportPlayerToSpawn(int client);
	
	// wins
	bool g_Complete[MAX_CLIENTS]; // which players won a microgame
	std::vector<int> finalWinners; // final winners

private:
	int m_state, m_warioState, m_microgame, m_round;
	int m_last_microgame[5];
	bool m_speedUp;
	float warioTimeLength;

	std::vector<Microgame*> m_microgames;
};
#endif
