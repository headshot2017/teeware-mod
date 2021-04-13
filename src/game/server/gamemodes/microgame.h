// WarioWare mod by Headshotnoby

#ifndef _MICROGAME_H
#define _MICROGAME_H

#include "WarioWare.h"
#include <game/server/gamecontext.h>

/*
	The base microgame class
	Make your own microgames by inheriting from this class.
*/

class CGameControllerWarioWare;

class Microgame
{
	CGameContext *m_pGameServer;
	IServer *m_pServer;
	CGameControllerWarioWare* m_pController;

public:
	// (~)Microgame()
	// The constructor and destructor. (duh)
	// Set up the m_microgameName and m_boss variables on your microgame's constructor.
	Microgame(CGameContext* pGameServer, CGameControllerWarioWare* pController)
	{
		m_pGameServer = pGameServer;
		m_pServer = m_pGameServer->Server();
		m_pController = pController;
	}
	virtual ~Microgame() {};

	// void Start()
	// Called when this microgame starts.
	// Set up objects, states etc.
	virtual void Start() = 0;

	// void End()
	// Called when this microgame ends.
	// Clean up objects, states etc.
	virtual void End() = 0;

	// void Tick()
	// Your microgame's update function.
	virtual void Tick() = 0;

	// int OnCharacterDeath(pVictim, pKiller, Weapon)
	// Called when a player dies.
	// Definition of this function in your microgame is optional.
	virtual int OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon) {return 0;}

	// int OnCharacterDamage(Victim, Killer, Dmg, Weapon)
	// Called when a player is hurt.
	// Definition of this function in your microgame is optional.
	virtual void OnCharacterDamage(int Victim, int Killer, int Dmg, int Weapon) {}

	// bool onChat(client, msg)
	// Called when a player sends a chat message.
	// Returns a boolean: if true, the message is suppressed and not displayed.
	// Definition of this function in your microgame is optional.
	virtual bool onChat(int client, const char *msg) {return false;}

	// void OnWinMicrogame(client, winTile)
	// Called when a player touches a win microgame entity.
	// winTile corresponds to the entity ID of the touched win tile on the game layer. (e.g. TILE_WARIOWARE_WIN, see mapitems.h)
	// Returns a bool: if true, allow them to win the microgame
	virtual bool OnWinMicrogame(int client, int winTile) {return true;}

	// void onBotInput(Input)
	// Called before sending input changes to the bot player with ID 63.
	// You can use this to implement an AI player of sorts in your microgame.
	virtual void OnBotInput(CNetObj_PlayerInput* Input) {mem_zero(Input, sizeof(CNetObj_PlayerInput));}

	const char *m_microgameName; // the microgame name
	bool m_boss; // if this microgame is a boss

protected:
	CGameControllerWarioWare *Controller() const { return m_pController; }
	CGameContext *GameServer() const { return m_pGameServer; }
	IServer *Server() const { return m_pServer; }
};

#endif // _MICROGAME_H