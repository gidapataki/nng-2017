#pragma once
#include "Client.h"
#include "parser.h"
#include "Matrix.h"
#include <vector>
#include <string>
#include <functional>


class Hypno : public CLIENT
{
public:
	Hypno();

	void AttackMove(int hero_id, const Position& pos);

	std::vector<MAP_OBJECT> GetEnemyObjects() const;

protected:
	virtual std::string GetPassword() { return std::string("c6gR92#1"); }
	virtual std::string GetPreferredOpponents() { return std::string("test"); }
	virtual bool NeedDebugLog() { return true; }
	virtual void Process();

	Matrix<int> GetHeatMap() const;

	std::vector<MAP_OBJECT> GetOurTurrets();
	std::vector<MAP_OBJECT> GetEnemyTurrets();
	std::vector<MAP_OBJECT> GetOurHeroes();
	std::vector<MAP_OBJECT> GetObjects(std::function<bool(const MAP_OBJECT&)> fn);
};
