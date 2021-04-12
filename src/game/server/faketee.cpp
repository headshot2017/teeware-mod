// BFS pathfinding code taken from another server of mine

#include <engine/shared/config.h>
#include <game/server/gamemodes/WarioWare.h>
#include "faketee.h"
#include "bfs.h"
#include <deque>
#include <math.h>
#include <base/math.h>
#include <string.h>

enum
{
	PATHRULE_NORMAL, // no rules
	PATHRULE_LAND // must land on ground first
};

std::vector<std::pair<vec2, int> > BotPaths[MAX_CLIENTS];
vec2 BotPathTarget[MAX_CLIENTS];
unsigned int BotPathInd[MAX_CLIENTS];
bool BotPathHook[MAX_CLIENTS];
int BotPathRule[MAX_CLIENTS];

std::deque<Bfs::Node*> currentNodes[MAX_CLIENTS]; // current nodes being looped through
std::map<vec2, Bfs::Node*> allNodes[MAX_CLIENTS]; // all nodes that were looped so they're ignored in the loop

namespace FakeTee
{
	void pathFind(CGameContext *GameServer, int BotID, vec2 target, int slowFind)
	{
		CPlayer *pbot = GameServer->m_apPlayers[MAX_CLIENTS-BotID-1];
		CCharacter *bot = (pbot) ? pbot->GetCharacter() : 0;

		if (not bot) return;

		for (unsigned i=0; i<BotPaths[BotID].size(); i++)
		{
			GameServer->Server()->SnapFreeID(BotPaths[BotID][i].second);
		}
		BotPaths[BotID].clear();

		if (GameServer->Collision()->CheckPoint(target)) // this position is inside a block, don't bother.
			return;

		// powered by breadth-first-search algorithm
		// thanks to http://qiao.github.io/PathFinding.js/visual for helping me visualize it

		vec2 botpos((int)(bot->m_Pos.x)/32, (int)(bot->m_Pos.y)/32);
		vec2 itarget((int)(target.x)/32, (int)(target.y)/32);
		BotPathTarget[BotID] = itarget;
		BotPathInd[BotID] = 0;
		BotPathHook[BotID] = false;
		BotPathRule[BotID] = PATHRULE_LAND;

		// clear old nodes
		currentNodes[BotID].clear();
		allNodes[BotID].clear();

		// add a node at the bot's pos and start from there
		Bfs::Node* startNode = new Bfs::Node(botpos, NULL);
		currentNodes[BotID].push_back(startNode);
		allNodes[BotID][botpos] = startNode;

		//pbot->m_PathFinding = slowFind;
		if (not slowFind)
		{
			while (not pathFindTick(GameServer, BotID));
			//printf("final size: %u\n", BotPaths[BotID].size());
			if (BotPaths[BotID].empty())
			{
				//SendChat(GameServer, BotID, "could not find path...");
			}
		}
	}

	bool pathFindTick(CGameContext *GameServer, int BotID, bool showResults)
	{
		CCharacter *bot = GameServer->GetPlayerChar(MAX_CLIENTS-BotID-1);
		if (not bot) return false;

		vec2 itarget = BotPathTarget[BotID];

		/*
		if (showResults)
		{
			for (unsigned i=0; i<BotPaths[BotID].size(); i++)
			{
				GameServer->Server()->SnapFreeID(BotPaths[BotID][i].second);
			}
			BotPaths[BotID].clear();
		}
		*/

		Bfs::Node* frontNode = currentNodes[BotID].front();
		currentNodes[BotID].pop_front();

		if ((int)(frontNode->m_Pos.x) == (int)(itarget.x) and (int)(frontNode->m_Pos.y) == (int)(itarget.y)) // we made it!
		{
			BotPaths[BotID].insert(BotPaths[BotID].begin(), std::pair<vec2,int>(frontNode->m_Pos*32, GameServer->Server()->SnapNewID()));
			Bfs::Node* winnerNode = frontNode->m_Parent;
			while (winnerNode)
			{
				BotPaths[BotID].insert(BotPaths[BotID].begin(), std::pair<vec2,int>(winnerNode->m_Pos*32, GameServer->Server()->SnapNewID()));
				winnerNode = winnerNode->m_Parent;
			}

			for (std::map<vec2, Bfs::Node*>::iterator iter = allNodes[BotID].begin(); iter != allNodes[BotID].end(); iter++)
			{
				if (iter->second)
				{
					delete iter->second;
					iter->second = NULL;
				}
			}
			allNodes[BotID].clear();

			return true;
		}

		int indexHere = GameServer->Collision()->GetMapIndex(frontNode->m_Pos*32);
		int tileHere = GameServer->Collision()->GetTileIndex(indexHere);

		vec2 left(frontNode->m_Pos.x-1, frontNode->m_Pos.y),
			 up(frontNode->m_Pos.x, frontNode->m_Pos.y-1),
			 right(frontNode->m_Pos.x+1, frontNode->m_Pos.y),
			 down(frontNode->m_Pos.x, frontNode->m_Pos.y+1);

		int tleft = GameServer->Collision()->GetTileIndex(GameServer->Collision()->GetMapIndex(left*32));
		int tup = GameServer->Collision()->GetTileIndex(GameServer->Collision()->GetMapIndex(up*32));
		int tright = GameServer->Collision()->GetTileIndex(GameServer->Collision()->GetMapIndex(right*32));
		int tdown = GameServer->Collision()->GetTileIndex(GameServer->Collision()->GetMapIndex(down*32));

		bool canDown = ((not allNodes[BotID].count(down) or allNodes[BotID][down]->m_Pos != down) and tdown != TILE_DEATH and not bot->GameLayerClipped(down*32) and not GameServer->Collision()->CheckPoint(down*32));
		bool canLeft = ((not allNodes[BotID].count(left) or allNodes[BotID][left]->m_Pos != left) and tleft != TILE_DEATH and not bot->GameLayerClipped(left*32) and not GameServer->Collision()->CheckPoint(left*32));
		bool canRight = ((not allNodes[BotID].count(right) or allNodes[BotID][right]->m_Pos != right) and tright != TILE_DEATH and not bot->GameLayerClipped(right*32) and not GameServer->Collision()->CheckPoint(right*32));
		bool canUp = ((not allNodes[BotID].count(up) or allNodes[BotID][up]->m_Pos != up) and tup != TILE_DEATH and not bot->GameLayerClipped(up*32) and not GameServer->Collision()->CheckPoint(up*32));

		int rule = BotPathRule[BotID];
		switch(rule)
		{
			case PATHRULE_NORMAL:
				{
					// freeze tiles
					if (tileHere == TILE_FREEZE)
					{
						// check if there are no unfreeze tiles right after
						if (canDown) canDown = (tdown == TILE_UNFREEZE);
						if (canLeft) canLeft = (tleft == TILE_UNFREEZE);
						if (canRight) canRight = (tright == TILE_UNFREEZE);
						if (canUp) canUp = (tup == TILE_UNFREEZE);

						if (not canDown and not canLeft and not canRight and not canUp)
							return currentNodes[BotID].empty(); // no unfreeze tiles around, stay away
					}

					/*
					// left, right and up platforming rules (grappling hook)
					// hook normally has a reach of 12 tiles long-ish

					int unhookablesLeft = 0;
					int unhookablesRight = 0;
					bool canLeft2 = true, canRight2 = true;
					for (int x=0; x<12; x++)
					{
						for (int y=1; y>-12; y--)
						{
							if ((x >= 10) and (y <= -10 or y >= 10)) // diagonal hook doesn't reach that far
							{
								unhookablesLeft++;
								unhookablesRight++;
								continue;
							}

							int tileLeft = GameServer->Collision()->GetCollisionAt((frontNode->m_Pos.x - x)*32, (frontNode->m_Pos.y + y)*32);
							int tileRight = GameServer->Collision()->GetCollisionAt((frontNode->m_Pos.x + x)*32, (frontNode->m_Pos.y + y)*32);
							bool hookableLeft = (tileLeft == TILE_SOLID or tileLeft == TILE_NOHOOK);
							bool hookableRight = (tileRight == TILE_SOLID or tileRight == TILE_NOHOOK);

							if (not hookableLeft) unhookablesLeft++;
							if (not hookableRight) unhookablesRight++;

							//if (unhookablesLeft >= 20 or unhookablesRight >= 20)
								//printf("%d %d %s %s\n", unhookablesLeft, unhookablesRight, canLeft?"true":"false", canRight?"true":"false");

							if (canLeft2 and unhookablesLeft >= 154) // no hookables on left side
							{
								canLeft2 = false;
							}
							if (canRight2 and unhookablesRight >= 154) // no hookables on right side
							{
								canRight2 = false;
							}
						}
					}
					if (not canLeft2 and not canRight2)
					{
						canLeft = false;
						canRight = false;
						canUp = false; // what's the point
					}
					*/
				}
				break;

			case PATHRULE_LAND:
				if (GameServer->Collision()->CheckPoint(down*32)) // reached the ground
				{
					BotPathRule[BotID] = PATHRULE_NORMAL;
					break;
				}
				canDown = true;
				canLeft = canRight = canUp = false;
				break;
		}

		

		// teleports
		CGameControllerWarioWare *Controller = (CGameControllerWarioWare *)GameServer->m_pController;

		int teleHere = max(GameServer->Collision()->IsTeleport(indexHere), GameServer->Collision()->IsEvilTeleport(indexHere));
		if (teleHere > 0) // it's a tele
		{
			for (unsigned i=0; i<Controller->m_TeleOuts[teleHere-1].size(); i++)
			{
				vec2 telepos((int)(Controller->m_TeleOuts[teleHere-1][i].x)/32,
					     (int)(Controller->m_TeleOuts[teleHere-1][i].y)/32);

				if (not allNodes[BotID].count(telepos)) // spawn a node in the other side
				{
					Bfs::Node* newNode = new Bfs::Node(telepos, frontNode);
					currentNodes[BotID].push_back(newNode);
					allNodes[BotID][telepos] = newNode;
				}
			}
			return currentNodes[BotID].empty(); // stop here because tele
		}



		// add possible nodes
		if (canDown)
		{
			Bfs::Node* newNode = new Bfs::Node(down, frontNode);
			currentNodes[BotID].push_back(newNode);
			allNodes[BotID][down] = newNode;
		}
		if (canLeft)
		{
			Bfs::Node* newNode = new Bfs::Node(left, frontNode);
			currentNodes[BotID].push_back(newNode);
			allNodes[BotID][left] = newNode;
		}
		if (canRight)
		{
			Bfs::Node* newNode = new Bfs::Node(right, frontNode);
			currentNodes[BotID].push_back(newNode);
			allNodes[BotID][right] = newNode;
		}
		if (canUp)
		{
			Bfs::Node* newNode = new Bfs::Node(up, frontNode);
			currentNodes[BotID].push_back(newNode);
			allNodes[BotID][up] = newNode;
		}

		/*
		if (showResults)
		{
			for (unsigned i=0; i<currentNodes[BotID].size(); i++)
			{
				BotPaths[BotID].insert(BotPaths[BotID].begin(), std::pair<vec2,int>(currentNodes[BotID][i]->m_Pos*32, GameServer->Server()->SnapNewID()));
			}
			BotPaths[BotID].insert(BotPaths[BotID].begin(), std::pair<vec2,int>(itarget*32, GameServer->Server()->SnapNewID()));
		}
		*/

		return currentNodes[BotID].empty();
	}

	bool pathFindMove(CGameContext *GameServer, int BotID, CNetObj_PlayerInput* Input)
	{
		CCharacter *Bot = GameServer->GetPlayerChar(MAX_CLIENTS-BotID-1);

		if (BotPathInd[BotID] >= BotPaths[BotID].size()) return true;

		vec2 currNode = BotPaths[BotID][BotPathInd[BotID]].first;
		int xdist = abs(currNode.x + 16 - Bot->m_Pos.x);
		int ydist = abs(currNode.y + 16 - Bot->m_Pos.y);
		int xdir = sign(currNode.x + 16 - Bot->m_Pos.x);
		int ydir = sign((int)(currNode.y) + 16+1 - (int)(Bot->m_Pos.y));

		Input->m_Direction = (xdist >= 4) ? xdir : 0;

		int jumpdist = (GameServer->Collision()->GetCollisionAt(Bot->m_Pos.x, Bot->m_Pos.y+32) == TILE_FREEZE) ? 16 : 30;

		if (ydir < 0 and ydist >= jumpdist) // jump!
		{
			int yvel = (int)(Bot->Core()->m_Vel.y);
			int oldjump = Input->m_Jump;

			if (not BotPathHook[BotID])
			{
				Input->m_Jump = (yvel >= 0) ? 1 : 0;
			}
			else
			{
				if (Bot->GetActiveWeapon() != WEAPON_NINJA)
				{
					if (not Input->m_Hook) // grab onto a wall
					{
						bool grabbed = false;
						for (int y=-11; y<=0; y++)
						{
							//for (int x=11; x>=0; x--)
							for (int x=0; x<12; x++)
							{
								if ((x >= 10) and (y <= -10 or y >= 10)) // diagonal hook doesn't reach that far
									continue;

								vec2 targetleft = Bot->m_Pos + (vec2(-x, y)*32);
								vec2 targetright = Bot->m_Pos + (vec2(x, y)*32);
								bool isSolidLeft = GameServer->Collision()->CheckPoint(targetleft);
								bool isSolidRight = GameServer->Collision()->CheckPoint(targetright);

								int tileleft, tileright;
								bool resultleft = (isSolidLeft and GameServer->Collision()->IntersectsLineToEnd(Bot->m_Pos, targetleft, &tileleft));
								bool resultright = (isSolidRight and GameServer->Collision()->IntersectsLineToEnd(Bot->m_Pos, targetright, &tileright));
								/*
								wallleft.x = (int)(wallleft.x/32); wallleft.y = (int)(wallleft.y/32);
								wallright.x = (int)(wallright.x/32); wallright.y = (int)(wallright.y/32);
								targetleft.x = (int)(targetleft.x/32); targetleft.y = (int)(targetleft.y/32);
								targetright.x = (int)(targetright.x/32); targetright.y = (int)(targetright.y/32);
								*/

								if ((not resultleft) and (not resultright)) // hit a wall early on, or none
									continue;

								vec2 target32 = (resultleft) ? targetleft : targetright;
								Input->m_TargetX = cos(atan2(target32.y - Bot->m_Pos.y, target32.x - Bot->m_Pos.x)) * 300;
								Input->m_TargetY = sin(atan2(target32.y - Bot->m_Pos.y, target32.x - Bot->m_Pos.x)) * 300;

								//printf("targetleft %d,%d resultleft %s - targetright %d,%d resultright %s\n", (int)targetleft.x/32, (int)targetleft.y/32, resultleft?"true":"false", (int)targetright.x/32, (int)targetright.y/32, resultright?"true":"false");
								//printf("%d %d, %d %d\n", Input->m_TargetX, Input->m_TargetY, x, y);

								Input->m_Hook = 1;
								grabbed = true;
								break;
							}
							if (grabbed) break;
						}
					}
					else
					{
						if (Bot->Core()->m_HookState == HOOK_GRABBED and Bot->Core()->m_HookedPlayer == -1)
						{
							if (Bot->m_Pos.y < Bot->Core()->m_HookPos.y) // hook line is horizontal or too far from path node
								Input->m_Hook = 0;
						}
					}
				}
				else
				{
					// we're a ninja, just fly lol
					vec2 botpos((int)(Bot->m_Pos.x)/32, (int)(Bot->m_Pos.y)/32);
					vec2 nodepos((int)(currNode.x)/32, (int)(currNode.y)/32);
					Input->m_TargetX = 1;
					Input->m_TargetY = -10000;
					Input->m_Fire++; // spam fire button
				}
			}

			if (GameServer->Collision()->CheckPoint(vec2(Bot->m_Pos.x-(xdir*14), Bot->m_Pos.y-32)))
				Input->m_Direction = xdir;

			if ((GameServer->Collision()->CheckPoint(Bot->m_Pos.x + 28.f / 2, Bot->m_Pos.y + 28.f / 2 + 5) or
			     GameServer->Collision()->CheckPoint(Bot->m_Pos.x - 28.f / 2, Bot->m_Pos.y + 28.f / 2 + 5)))
			{
				if (oldjump) Input->m_Jump = Input->m_Hook = 0;
				BotPathHook[BotID] = false;
			}
			else if (Bot->Core()->m_JumpedTotal >= Bot->Core()->m_Jumps-1 and yvel > 0)
				BotPathHook[BotID] = true;
		}
		else // don't jump or hook
			Input->m_Jump = Input->m_Hook = 0;


		if (xdist <= 16 and ydist < 16)
			BotPathInd[BotID]++;

		return BotPathInd[BotID] >= BotPaths[BotID].size();
	}
}
