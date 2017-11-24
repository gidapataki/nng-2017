#include "Hypno.h"
#include <set>


Hypno::Hypno() {
	// nothing to see here
}

void Hypno::AttackMove(int hero_id, const Position& pos) {

}

std::vector<MAP_OBJECT> Hypno::GetEnemyObjects() const {
	std::vector<MAP_OBJECT> objects;
	for (auto& obj : mParser.Units) {
		if (obj.side != 0) {
			objects.push_back(obj);
		}
	}
	return objects;
}

void Hypno::Process() {
}

CLIENT* CreateClient()
{
	return new Hypno();
}

std::vector<MAP_OBJECT> Hypno::GetObjects(std::function<bool(const MAP_OBJECT&)> fn) {
	std::vector<MAP_OBJECT> vec;
	for (auto& unit : mParser.Units) {
		if (fn(unit)) {
			vec.push_back(unit);
		}
	}

	return vec;
}

std::vector<MAP_OBJECT> Hypno::GetOurHeroes() {
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

std::vector<MAP_OBJECT> Hypno::GetOurTurrets() {
	std::vector<MAP_OBJECT> vec;
	for (auto& unit : mParser.Units) {
		if (unit.t == TURRET && unit.side == 0) {
			vec.push_back(unit);
		}
	}
	return vec;
}

std::vector<MAP_OBJECT> Hypno::GetEnemyTurrets() {
	std::vector<MAP_OBJECT> vec;
	for (auto& unit : mParser.Units) {
		if (unit.t == TURRET && unit.side == 1) {
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

	return result;
}
