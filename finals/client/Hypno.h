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

protected:
	virtual std::string GetPassword() { return std::string("c6gR92#1"); }
	virtual std::string GetPreferredOpponents() { return std::string("test"); }
	virtual bool NeedDebugLog() { return true; }
	virtual void Process();

	void AttackMove(int hero_id, const Position& pos);
	void AttackTop(const MAP_OBJECT& hero);

	Matrix<double> GetDamageMap() const;
	Matrix<double> GetHPMap() const;
	Matrix<double> GetHeatMap() const;
	Matrix<double> GetTowerHeatMap() const;
	Matrix<double> GetUnitHeatMap() const;

	std::vector<MAP_OBJECT> GetOurTurrets() const;
	std::vector<MAP_OBJECT> GetEnemyTurrets() const;
	std::vector<MAP_OBJECT> GetHeroes(int side) const;
	std::vector<MAP_OBJECT> GetOurHeroes() const;
	std::vector<MAP_OBJECT> GetEnemyHeroes() const;
	std::vector<MAP_OBJECT> GetMinions(int side) const;
	std::vector<MAP_OBJECT> GetOurMinions() const;
	std::vector<MAP_OBJECT> GetEnemyMinions() const;
	std::vector<MAP_OBJECT> GetObjects(std::function<bool(const MAP_OBJECT&)> fn) const;
	std::vector<MAP_OBJECT> GetEnemyObjects() const;
	std::vector<MAP_OBJECT> GetOurObjects() const;
	std::vector<MAP_OBJECT> GetEnemyObjectsNear(
		const Position& pos, int distance_sq) const;
	MAP_OBJECT GetEnemyBase() const;
	std::vector<MAP_OBJECT> GetObjectsNear(
		const Position& pos, int distance_sq) const;

	bool CanOneHit(const MAP_OBJECT& unit) const;

	int GetPreferredEnemyToAttack(const std::vector<MAP_OBJECT>& enemies) const;

	bool IsTopLane(const Position& pos) const;
	bool IsLeftLane(const Position& pos) const;
	bool IsBottomLane(const Position& pos) const;
	bool IsRightLane(const Position& pos) const;

	std::vector<MAP_OBJECT> GetTopEnemyTurrets() const;

	std::vector<MAP_OBJECT> OrderByX(std::vector<MAP_OBJECT> units, bool reverse=false) const;
	std::vector<MAP_OBJECT> OrderByY(std::vector<MAP_OBJECT> units, bool reverse=false) const;

	static bool LessByX(const MAP_OBJECT& lhs, const MAP_OBJECT& rhs);
	static bool LessByY(const MAP_OBJECT& lhs, const MAP_OBJECT& rhs);
	static bool LessByHp(const MAP_OBJECT& lhs, const MAP_OBJECT& rhs);

	int MaxX() const;
	int MaxY() const;
};
