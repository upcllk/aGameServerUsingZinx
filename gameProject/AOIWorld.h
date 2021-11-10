#pragma once
#include <list>
#include <vector>

class Player {
public:
	virtual int GetX() = 0;
	virtual int GetY() = 0;
	int test = 0;
};

class Grid {
public:
	std::list<Player*> m_players;
};

class AOIWorld
{
	int x_begin = 0;
	int x_end = 0;
	int y_begin = 0;
	int y_end = 0;
	// 横纵轴都要分成几份
	int x_count = 0;
	int y_count = 0;
	int x_width = 0;
	int y_width = 0;
public:
	AOIWorld(int _x_begin, int _x_end, int _y_begin, int _y_end,
		int _x_count, int _y_count);
	std::vector<Grid> m_grids;
	// 获取周围玩家
	std::list<Player*> GetSrdPlayers(Player* _player);
	// 添加新玩家到 AOI 网格
	bool AddPlayer(Player* _player);
	// 摘除玩家
	void DelPlayer(Player* _player);
	
};

