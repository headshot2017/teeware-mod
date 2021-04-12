#include "gamecontext.h"
#include "player.h"

namespace FakeTee
{
	void pathFind(CGameContext *GameServer, int BotID, vec2 target, int slowFind=0);
	bool pathFindTick(CGameContext *GameServer, int BotID, bool showResults=false);
	bool pathFindMove(CGameContext *GameServer, int BotID, CNetObj_PlayerInput* Input);
	//std::vector<std::pair<vec2, int>>* getPath(int BotID);
}
