#pragma once
#include "Position.h"
#include <vector>
#include <string>

static const int BASE_MAX_HP = 10000;
static const int TURRET_MAX_HP = 2000;
static const int MINION_MAX_HP = 200;
static const int HERO_MAX_HP_BASE = 400;
static const int HERO_MAX_HP_PER_LEVEL = 10;
static const int HERO_DAMAGE_BASE = 25;
static const double HERO_DAMAGE_PER_LEVEL = 0.5;
static const int HERO_RANGE_SQ = 13;
static const int TURRET_RANGE_SQ = 13;
static const int MINION_RANGE_SQ = 8;

enum UNIT_TYPE {
	HERO,
	BASE,
	TURRET,
	MINION
};

struct MAP_OBJECT {
	int id, hp, side;
	Position pos;
	UNIT_TYPE t;
};
struct ATTACK_INFO {
	int attacker_id;
	Position attacker_pos;
	int target_id;
	Position target_pos;
};
struct RESPAWN_INFO
{
	int hero_id;
	int side;
	int tick;
};
struct CONTROLLER_INFO
{
	int hero_id;
	int controller_id; // 0 if you are the controller. 100 if it's manually controlled with the viewer in test mode. player_id otherwise.
};
struct PLAYER_INFO
{
	int player_id;
	int match_wins;
	int elo_points;
	int qualified;
	std::string name;
};

class PARSER
{
public:
	PARSER();
	// received after connecting:
	enum GROUND_TYPE
	{
		EMPTY,
		WALL
	};
	int w, h;
	std::vector<GROUND_TYPE> Arena;
	std::vector<PLAYER_INFO> Players;

	// received each tick:
	int tick;
	int match_id;
	enum MATCH_TYPE
	{
		MELEE,
		DUEL
	};
	MATCH_TYPE match_type;
	int level[2]; // your and enemy hero levels

	std::vector<MAP_OBJECT> Units;
	std::vector<CONTROLLER_INFO> Controllers;
	std::vector<ATTACK_INFO> Attacks;
	std::vector<RESPAWN_INFO> Respawns;

	GROUND_TYPE GetAt(const Position &p) const { return p.x<w && p.y<h ? Arena[p.x + p.y*w] : WALL; }
	MAP_OBJECT *GetUnitByID(int id);
	PLAYER_INFO *GetPlayerByID(int player_id);
	enum MATCH_RESULT {
		ONGOING,
		VICTORY,
		DRAW,
		DEFEAT
	};
	MATCH_RESULT match_result;
	void Parse(const std::vector<std::string> &ServerResponse);
	void ParseMap(const std::vector<std::string> &ServerResponse);
	void ParsePlayers(const std::vector<std::string> &ServerResponse);

	int GetHeroDamage(int side) const;
	int GetOurHeroDamage() const;
	int GetEnemyHeroDamage() const;

	int GetHeroMaxHP(int side) const;
	int GetOurHeroMaxHP() const;
	int GetEnemyHeroMaxHP() const;

	int GetMaxHPOfunit(const MAP_OBJECT& unit) const;
};
