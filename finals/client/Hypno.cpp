#include "Hypno.h"
#include <set>
#include <iostream>
#include <algorithm>


CLIENT* CreateClient()
{
	return new Hypno();
}

Hypno::Hypno() {
	// nothing to see here
}

void Hypno::AttackMove(int hero_id, const Position& pos) {
	auto hero = mParser.GetUnitByID(hero_id);
	auto possible_targets = GetEnemyObjectsNear(hero->pos, HERO_RANGE_SQ);
	if (!possible_targets.empty()) {
		// TODO sort somehow
		Attack(hero_id, possible_targets.front().id);
	} else {
		Move(hero_id, mDistCache.GetNextTowards(hero->pos, pos));
	}
}

void Hypno::AttackTop(const MAP_OBJECT& hero) {
	// if ()
}

void Hypno::Process() {
	for (auto& hero : GetOurHeroes()) {
		AttackMove(hero.id, GetEnemyBase().pos);
	}
}

std::vector<MAP_OBJECT> Hypno::GetOurHeroes() const {
	std::vector<MAP_OBJECT> vec;
	std::set<int> ids;

	for (auto& cc : mParser.Controllers) {
		if (cc.controller_id == 0) {
			ids.insert(cc.hero_id);
		}
	}

	return GetObjects([&](const MAP_OBJECT& unit) {
		return (unit.t == HERO && ids.count(unit.id) > 0);
	});
}

std::vector<MAP_OBJECT> Hypno::GetOurTurrets() const {
	return GetObjects([](const MAP_OBJECT& unit) {
		return unit.t == TURRET && unit.side == 0;
	});
}

std::vector<MAP_OBJECT> Hypno::GetEnemyTurrets() const {
	return GetObjects([](const MAP_OBJECT& unit) {
		return unit.t == TURRET && unit.side == 1;
	});
}

std::vector<MAP_OBJECT> Hypno::GetEnemyObjects() const {
	return GetObjects([](const MAP_OBJECT& obj) {
		return obj.side != 0;
	});
}

std::vector<MAP_OBJECT> Hypno::GetEnemyObjectsNear(
	const Position& pos, int distance_sq) const
{
	return GetObjects([&](const MAP_OBJECT& obj) {
		return obj.side != 0 && pos.DistSquare(obj.pos) <= distance_sq;
	});
}

MAP_OBJECT Hypno::GetEnemyBase() const {
	for (auto& unit : mParser.Units) {
		if (unit.t == UNIT_TYPE::BASE && unit.side != 0) {
			return unit;
		}
	}
	std::cerr << "No enemy base found :scream:" << std::endl;
	return {};
}

std::vector<MAP_OBJECT> Hypno::GetObjectsNear(
	const Position& pos, int distance_sq) const
{
	return GetObjects([&](const MAP_OBJECT& obj) {
		return pos.DistSquare(obj.pos) <= distance_sq;
	});
}

std::vector<MAP_OBJECT> Hypno::GetObjects(std::function<bool(const MAP_OBJECT&)> fn) const {
	std::vector<MAP_OBJECT> vec;
	for (auto& unit : mParser.Units) {
		if (fn(unit)) {
			vec.push_back(unit);
		}
	}

	return vec;
}

Matrix<int> Hypno::GetHeatMap() const {
	Matrix<int> result{
			static_cast<Matrix<int>::size_type>(mParser.w),
			static_cast<Matrix<int>::size_type>(mParser.h),
			0
	};

	static constexpr int friendly = 10;
	static constexpr int enemy = 20;

	static constexpr int tower = 40;
	static constexpr int minion = 5;

	static constexpr int neighbouring = 1;

	for (const auto& enemyTurret: GetEnemyTurrets()) {
		for (const auto& cell: GetObjectsNear(enemyTurret.pos, TURRET_RANGE_SQ))
		{
			result[cell.pos] += enemy * tower;
		}
	}

	for (const auto& ourTurret: GetOurTurrets()) {
		for (const auto& cell: GetObjectsNear(ourTurret.pos, TURRET_RANGE_SQ))
		{
			result[cell.pos] -= friendly * tower;
		}
	}

	return result;
}

bool Hypno::IsTopLane(const Position& pos) const {
	return pos.y > MaxY() - 4;
}

bool Hypno::IsBottomLane(const Position& pos) const {
	return pos.y < 4;
}

bool Hypno::IsLeftLane(const Position& pos) const {
	return pos.x < 4;
}

bool Hypno::IsRightLane(const Position& pos) const {
	return pos.x > MaxX() - 4;
}

std::vector<MAP_OBJECT> Hypno::GetTopEnemyTurrets() const {
	return OrderByX(GetObjects([&](const MAP_OBJECT& unit) {
		return unit.t == TURRET && unit.side == 1 && IsTopLane(unit.pos);
	}));
}

std::vector<MAP_OBJECT> Hypno::OrderByX(
	std::vector<MAP_OBJECT> units, bool reverse) const
{
	std::sort(units.begin(), units.end(), LessByX);
	if (reverse) {
		std::reverse(units.begin(), units.end());
	}
	return units;
}

std::vector<MAP_OBJECT> Hypno::OrderByY(
	 std::vector<MAP_OBJECT> units, bool reverse) const
{
	std::sort(units.begin(), units.end(), LessByY);
	if (reverse) {
		std::reverse(units.begin(), units.end());
	}
	return units;
}

bool Hypno::LessByX(const MAP_OBJECT& lhs, const MAP_OBJECT& rhs) {
	return lhs.pos.x < rhs.pos.x;
}

bool Hypno::LessByY(const MAP_OBJECT& lhs, const MAP_OBJECT& rhs) {
	return lhs.pos.y < rhs.pos.y;
}

int Hypno::MaxX() const {
	return mParser.w - 1;
}
int Hypno::MaxY() const {
	return mParser.h - 1;
}

