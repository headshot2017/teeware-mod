#include <engine/shared/config.h>
#include "math.h" // not the C math lib.

MGMath::MGMath(CGameContext* pGameServer, CGameControllerWarioWare* pController) : Microgame(pGameServer, pController)
{
	m_microgameName = "math";
	m_boss = false;
}

void MGMath::Start()
{
	char operations[4] = {'+', '-', '*', '/'};
	char aBuf[128];
	m_winFirst = false;
	m_operation = operations[rand() % 4];
	
	if (m_operation == '*') // easy numbers
	{
		m_num1 = 2 + (rand() % 9);
		m_num2 = 1 + (rand() % 10);
	}
	else if (m_operation == '-')
	{
		m_num1 = rand() % 19 + 1;
		do m_num2 = rand() % 10 + 1;
		while (m_num2 > m_num1);
	}
	else if (m_operation == '+')
	{
		m_num1 = rand() % 20;
		m_num2 = 1 + (rand() % 10);
	}
	else
	{
		m_num2 = (rand() % 8) + 1;
		m_num1 = ((rand() % 8) + 1) * m_num2;
	}
	
	str_format(aBuf, sizeof(aBuf), "Type the answer!\n %d%c%d = ?", m_num1, m_operation, m_num2);
	GameServer()->SendBroadcast(aBuf, -1);
	
	int snd1[2] = {g_Config.m_WwSndMgAnswer1_Offset, g_Config.m_WwSndMgAnswer2_Offset};
	int snd2[2] = {g_Config.m_WwSndMgAnswer1_Length, g_Config.m_WwSndMgAnswer2_Length};
	int ind = rand() % 2;
	Controller()->setPlayerTimers(snd1[ind], snd2[ind]);
}

void MGMath::End()
{
	// nothing to clean
}

void MGMath::Tick()
{
	// nothing to tick
}

bool MGMath::onChat(int client, const char *msg)
{
	int pAnswer = atoi(msg);
	if ((m_operation == '+' and m_num1+m_num2 == pAnswer) or
		(m_operation == '-' and m_num1-m_num2 == pAnswer) or
		(m_operation == '*' and m_num1*m_num2 == pAnswer) or
		(m_operation == '/' and m_num1/m_num2 == pAnswer))
	{
		if (not m_winFirst)
		{
			m_winFirst = true;
			char aBuf[128];
			str_format(aBuf, sizeof(aBuf), "%s answered first!", GameServer()->Server()->ClientName(client));
			GameServer()->SendChat(-1, CGameContext::CHAT_ALL, aBuf);
		}
		Controller()->winMicroGame(client);
		return true;
	}
	return false;
}
