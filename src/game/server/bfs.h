// breadth first search pathfinding algorithm

#include <base/vmath.h>

namespace Bfs
{
	class Node
	{
	public:
		Node(vec2 pos, Node* parent) {m_Pos = pos; m_Parent = parent;}
		vec2 m_Pos;
		Node* m_Parent; // previous node

		bool operator<(const vec2 &other) const
		{
			return m_Pos.x < other.x || m_Pos.y < other.y;
		}
	};
}
