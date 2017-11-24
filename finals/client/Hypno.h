#pragma once
#include "Client.h"
#include "parser.h"
#include "Matrix.h"
#include <vector>
#include <map>
#include <string>
#include <functional>


class Hypno : public CLIENT
{
public:
	Hypno(std::string preferredOpponents="test");

protected:
	virtual std::string GetPassword() { return std::string("c6gR92#1"); }
	virtual std::string GetPreferredOpponents() {
		return mPreferredOpponents;
	}
	virtual bool NeedDebugLog() { return true; }
	virtual void Process();
	void MatchEnd() override;
	void UpdateEnemyHeroes();
	std::map<int, int> GetMostEvilEnemyHeroes() const;

	void AttackMove(int hero_id, const Position& pos);
	void AttackTop(const MAP_OBJECT& hero);
	void AttackDown(const MAP_OBJECT& hero);
	void AttackMid(const MAP_OBJECT& hero);
	void AttackInside(const MAP_OBJECT& hero);

	Matrix<double> GetDamageMap(const std::vector<MAP_OBJECT>& units) const;
	Matrix<double> GetDamageMap() const;
	Matrix<double> GetHPMap() const;
	Matrix<double> GetHeatMap() const;
	Matrix<double> GetTowerHeatMap() const;
	Matrix<double> GetUnitHeatMap() const;

	std::vector<MAP_OBJECT> GetOurTurrets() const;
	std::vector<MAP_OBJECT> GetEnemyTurrets() const;
	std::vector<MAP_OBJECT> GetControlledHeroes() const;
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

	std::vector<Position> GetNeighbours(const Position& pos) const;
	bool IsNeighbourOfCircle(
		const Position& pos, const Position& center, int radius_sq) const;

	bool CanOneHit(const MAP_OBJECT& unit) const;
	Position Retreat(const Matrix<double>& dmg_map, const MAP_OBJECT& hero) const;
	Position FightOrFlight(int hero_id) const;

	int GetPreferredEnemyToAttack(const std::vector<MAP_OBJECT>& enemies) const;

	bool IsTopLane(const Position& pos) const;
	bool IsLeftLane(const Position& pos) const;
	bool IsBottomLane(const Position& pos) const;
	bool IsRightLane(const Position& pos) const;

	std::vector<MAP_OBJECT> GetTopEnemyTurrets() const;
	std::vector<MAP_OBJECT> GetRightEnemyTurrets() const;
	std::vector<MAP_OBJECT> GetMidEnemyTurrets() const;

	std::vector<MAP_OBJECT> OrderByX(std::vector<MAP_OBJECT> units, bool reverse=false) const;
	std::vector<MAP_OBJECT> OrderByY(std::vector<MAP_OBJECT> units, bool reverse=false) const;
	std::vector<MAP_OBJECT> OrderByDst(std::vector<MAP_OBJECT> units, bool reverse=false) const;

	std::vector<MAP_OBJECT> GetTopMinions() const;
	std::vector<MAP_OBJECT> GetDownMinions() const;
	std::vector<MAP_OBJECT> GetMidMinions() const;

	static bool LessByX(const MAP_OBJECT& lhs, const MAP_OBJECT& rhs);
	static bool LessByY(const MAP_OBJECT& lhs, const MAP_OBJECT& rhs);
	static bool LessByHp(const MAP_OBJECT& lhs, const MAP_OBJECT& rhs);

	using ObjectCompare = std::function<bool(const MAP_OBJECT&, const MAP_OBJECT&)>;
	ObjectCompare LessByDst() const;

	int MaxX() const;
	int MaxY() const;

	int GetLane(const Position& pos) const;
	int GetAdvance(const Position& pos) const;
	int PreferLane(const MAP_OBJECT& hero) const;
	bool IsNearOurBase(const MAP_OBJECT& unit, int dst=13) const;

	bool IsAtTop(const MAP_OBJECT& unit) const;
	bool IsAtDown(const MAP_OBJECT& unit) const;
	bool IsAtMid(const MAP_OBJECT& unit) const;
	bool HasTopHero() const;
	bool HasDownHero() const;
	bool IsEnemyInside() const;

	std::string mPreferredOpponents;
	std::map<int, int> mSuccesfulEnemyHeroes;
	std::map<int, Position> mLastPositionOfMinions;
};
