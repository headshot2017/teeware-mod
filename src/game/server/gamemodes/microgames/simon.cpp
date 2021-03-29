// WarioWare mod by Headshotnoby

#include <engine/shared/config.h>
#include "simon.h"

const char *simonNames[] = {"Simon", "Someone"};
const char *simonModes[] = {"Jump", "Look up", "Look down"};
const float PI = 3.141592653589793f;

MGSimon::MGSimon(CGameContext* pGameServer, CGameControllerWarioWare* pController) : Microgame(pGameServer, pController)
{
	m_microgameName = "simon";
	m_boss = false;
}

void MGSimon::Start()
{
	m_Someone = rand() % 2; // simon/someone
	m_SimonMode = rand() % 3; // simons[] array
	
	for (int i=0; i<MAX_CLIENTS; i++)
		Controller()->g_Complete[i] = (m_Someone) ? true : false;
	
	char aBuf[96];
	str_format(aBuf, sizeof(aBuf), "%s says: %s!", simonNames[m_Someone], simonModes[m_SimonMode]);
	GameServer()->SendBroadcast(aBuf, -1);

	Controller()->setPlayerTimers(g_Config.m_WwSndMgSimonSays_Offset, g_Config.m_WwSndMgSimonSays_Length);
}

void MGSimon::End()
{
	// nothing to clean
}

void MGSimon::Tick()
{
	float timeLeft = Controller()->getTimeLength() - Controller()->getTimer();

	if (timeLeft < 2950 and timeLeft > 200)
	{
		for (int i=0; i<MAX_CLIENTS; i++)
		{
			CCharacter *Char = GameServer()->GetPlayerChar(i);
			if (not Char) continue;

			CNetObj_PlayerInput* input = Char->GetInput();
			float angle = -atan2(input->m_TargetY, input->m_TargetX) / PI * 180;
		
			if ((m_SimonMode == 0 and input->m_Jump&1) or
				(m_SimonMode == 1 and angle >= 75 and angle < 105) or // up
				(m_SimonMode == 2 and angle <= -75 and angle > -105)) // down
			{
				if (m_Someone) // simon didn't say it
				{
					Controller()->g_Complete[i] = false;
					Char->Die(i, WEAPON_WORLD, timeLeft/1000.f);
					GameServer()->SendChatTarget(i, "Simon didn't say it!...");
				}
				else
					Controller()->winMicroGame(i);
			}
		}
	}
}