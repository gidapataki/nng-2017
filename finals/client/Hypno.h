#pragma once
#include "Client.h"
#include "parser.h"
#include "Matrix.h"
#include <vector>
#include <string>


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

	std::vector<MAP_OBJECT> GetOurTurrets();
	std::vector<MAP_OBJECT> GetEnemyTurrets();
	std::vector<int> GetOurHeroes();
	Matrix<int> GetHeatMap() const;
};
