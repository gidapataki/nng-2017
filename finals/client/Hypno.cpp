#include "Hypno.h"
#include <set>
#include <map>
#include <vector>
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
		Attack(hero_id, GetPreferredEnemyToAttack(possible_targets));
	} else {
		Move(hero_id, mDistCache.GetNextTowards(hero->pos, pos));
	}
}

void Hypno::AttackTop(const MAP_OBJECT& hero) {
	if (hero.pos.y < MaxY() - 4 && hero.pos.x < 6) {
		AttackMove(hero.id, {4, MaxY() - 4});
	} else {
		auto turrets = GetTopEnemyTurrets();
		if (turrets.empty()) {
			AttackMove(hero.id, {MaxX() - 1, MaxY() - 1});
		} else {
			auto target = turrets[0].pos;
			target.x -= 3;
			AttackMove(hero.id, target);
		}
	}
}

void Hypno::Process() {
	for (auto& hero : GetOurHeroes()) {
		AttackTop(hero);
	}
}

std::vector<MAP_OBJECT> Hypno::GetHeroes(int side) const {
	std::vector<MAP_OBJECT> vec;
	std::set<int> ids;

	for (auto& cc : mParser.Controllers) {
		if (cc.controller_id == side) {
			ids.insert(cc.hero_id);
		}
	}

	return GetObjects([&](const MAP_OBJECT& unit) {
		return (unit.t == HERO && ids.count(unit.id) > 0);
	});
}

std::vector<MAP_OBJECT> Hypno::GetOurHeroes() const {
	return GetHeroes(0);
}

std::vector<MAP_OBJECT> Hypno::GetEnemyHeroes() const {
	return GetHeroes(1);
}

std::vector<MAP_OBJECT> Hypno::GetMinions(int side) const {
	std::vector<MAP_OBJECT> vec;
	std::set<int> ids;

	for (auto& cc : mParser.Controllers) {
		if (cc.controller_id == side) {
			ids.insert(cc.hero_id);
		}
	}

	return GetObjects([&](const MAP_OBJECT& unit) {
		return (unit.t == MINION && ids.count(unit.id) > 0);
	});
}

std::vector<MAP_OBJECT> Hypno::GetOurMinions() const {
	return GetMinions(0);
}

std::vector<MAP_OBJECT> Hypno::GetEnemyMinions() const {
	return GetMinions(1);
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

std::vector<MAP_OBJECT> Hypno::GetOurObjects() const {
	return GetObjects([](const MAP_OBJECT& obj) {
		return obj.side == 0;
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

bool Hypno::CanOneHit(const MAP_OBJECT& unit) const {
	return unit.hp <= mParser.GetHeroDamage(!unit.side);
}

int Hypno::GetPreferredEnemyToAttack(const std::vector<MAP_OBJECT>& enemies) const {
	assert(!enemies.empty());
	// one hit a turret if we can
	for (auto& enemy : enemies) {
		if (enemy.t == UNIT_TYPE::TURRET && CanOneHit(enemy)) {
			return enemy.id;
		}
	}

	// one hit a hero if we can
	for (auto& enemy : enemies) {
		if (enemy.t == UNIT_TYPE::HERO && CanOneHit(enemy)) {
			return enemy.id;
		}
	}

	// one hit a minion if we can
	for (auto& enemy : enemies) {
		if (enemy.t == UNIT_TYPE::MINION && CanOneHit(enemy)) {
			return enemy.id;
		}
	}

	// hit the unit with the least hp
	return std::min_element(begin(enemies), end(enemies), LessByHp)->id;
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

Matrix<double> Hypno::GetDamageMap() const {
	Matrix<double> result{
		static_cast<Matrix<double>::size_type>(mParser.w),
		static_cast<Matrix<double>::size_type>(mParser.h),
		0
	};


	for (auto& unit : mParser.Units) {
		// skip bases for now
		if (unit.t == UNIT_TYPE::BASE) {
			continue;
		}

		int sign = (unit.side == 0 ? -1 : 1);

		int range_sq = mParser.GetAttackRangeSquaredOfUnit(unit);
		int dmg = mParser.GetDamageOfUnit(unit);

		const int radius = 6;
		for (int y = unit.pos.y - radius; y <= unit.pos.y + radius; ++y) {
			for (int x = unit.pos.x - radius; x <= unit.pos.x + radius; ++x) {
				if (x < 0 || x >= MaxX() || y < 0 || y >= MaxY()) {
					continue;
				}
				int d_sq = Position{x, y}.DistSquare(unit.pos);
				if (d_sq > range_sq) {
					continue;
				}
				if (x == 2 && y == result.height() - 1) {
					assert(false);
				}
				result.at(x, y) += sign * dmg;
			}
		}
	}

	return result;
}

Matrix<double> Hypno::GetHPMap() const {
	Matrix<double> result{
		static_cast<Matrix<double>::size_type>(mParser.w),
		static_cast<Matrix<double>::size_type>(mParser.h),
		0
	};


	for (auto& unit : mParser.Units) {
		// skip bases for now
		if (unit.t == UNIT_TYPE::BASE) {
			continue;
		}

		int sign = (unit.side == 0 ? -1 : 1);

		int range_sq = mParser.GetAttackRangeSquaredOfUnit(unit);
		int hp = unit.hp;

		const int radius = 6;
		for (int y = unit.pos.y - radius; y <= unit.pos.y + radius; ++y) {
			for (int x = unit.pos.x - radius; x <= unit.pos.x + radius; ++x) {
				if (x < 0 || x > MaxX() || y < 0 || y > MaxY()) {
					continue;
				}
				int d_sq = Position{x, y}.DistSquare(unit.pos);
				if (d_sq > range_sq) {
					continue;
				}
				result(x, y) += sign * hp;
			}
		}
	}

	return result;
}

Matrix<double> Hypno::GetHeatMap() const {
	return GetTowerHeatMap() + GetUnitHeatMap();
}

Matrix<double> Hypno::GetTowerHeatMap() const {
	Matrix<double> result{
			static_cast<Matrix<double>::size_type>(mParser.w),
			static_cast<Matrix<double>::size_type>(mParser.h),
			0
	};

	static constexpr int friendly = 10;
	static constexpr int enemy = 20;

	static constexpr int tower = 40;

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

Matrix<double> Hypno::GetUnitHeatMap() const {
	Matrix<double> result{
			static_cast<Matrix<double>::size_type>(mParser.w),
			static_cast<Matrix<double>::size_type>(mParser.h),
			0
	};

	static constexpr int friendly = 10;
	static constexpr int enemy = 20;

	static constexpr int minion = 5;
	static constexpr int hero = 10; // TODO: Make this depend on level/hp
	static constexpr int effectWidth = 5;

	std::map<Position, int> sources;
	for (const auto& ourHero: GetOurHeroes()) {
		result[ourHero.pos] -= friendly * hero;
		sources[ourHero.pos] = result[ourHero.pos];
	}

	for (const auto& enemyHero: GetEnemyHeroes()) {
		result[enemyHero.pos] += enemy * hero;
		sources[enemyHero.pos] = result[enemyHero.pos];
	}

	for (const auto& ourMinion: GetOurMinions()) {
		result[ourMinion.pos] -= friendly * minion;
		sources[ourMinion.pos] = result[ourMinion.pos];
	}

	for (const auto& enemyMinion: GetEnemyMinions()) {
		result[enemyMinion.pos] += enemy * minion;
		sources[enemyMinion.pos] = result[enemyMinion.pos];
	}

	for (const auto& source: sources) {
		const auto& sourceX = source.first.x;
		const auto& sourceY = source.first.y;
		for (int x = sourceX-effectWidth; x < sourceX+effectWidth + 1; ++x) {
			for (int y = sourceY-effectWidth; y < sourceY+effectWidth + 1; ++y)
			{
				if (x < 0 || x > MaxX() || y < 0 || y > MaxY()) {
					continue;
				}
				auto currentPosition = Position{x, y};
				auto distance = std::abs(
						mDistCache.GetDist(currentPosition, source.first));
				result[currentPosition] +=
						(double(effectWidth - distance)/effectWidth) *
						double(source.second);
			}
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

bool Hypno::LessByHp(const MAP_OBJECT& lhs, const MAP_OBJECT& rhs) {
	return lhs.hp < rhs.hp;
}

int Hypno::MaxX() const {
	return mParser.w - 1;
}

int Hypno::MaxY() const {
	return mParser.h - 1;
}

