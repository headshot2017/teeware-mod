#ifndef _MICROGAME_MATH_H
#define _MICROGAME_MATH_H

#include "../microgame.h"

/*
	Math microgame.
	Addition, substraction, multiplication, division.
	2+2=4-1=3 Quick maths.
*/

class MGMath : public Microgame
{
public:
	MGMath(CGameContext* pGameServer, CGameControllerWarioWare* pController);
	~MGMath() {}

	void Start();
	void End();
	void Tick();
	bool onChat(int client, const char *msg);

private:
	char m_operation; // one of four arithmetic operations.
	int m_num1, m_num2; // two numbers.
	bool m_winFirst; // player answered first
};

#endif // _MICROGAME_MATH_H