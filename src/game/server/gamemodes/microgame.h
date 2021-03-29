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
	virtual int OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon) = 0;

	// int OnCharacterDamage(Victim, Killer, Dmg, Weapon)
	// Called when a player is hurt.
	virtual void OnCharacterDamage(int Victim, int Killer, int Dmg, int Weapon) = 0;

	// bool onChat(client, msg)
	// Called when a player sends a chat message.
	// Returns a boolean: if false, the message is suppressed and not displayed.
	virtual bool onChat(int client, const char *msg) = 0;

	const char *m_microgameName; // the microgame name
	bool m_boss; // if this microgame is a boss

protected:
	CGameControllerWarioWare *Controller() const { return m_pController; }
	CGameContext *GameServer() const { return m_pGameServer; }
	IServer *Server() const { return m_pServer; }
};

#endif // _MICROGAME_H