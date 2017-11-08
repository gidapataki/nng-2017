#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <set>
#include <array>
#include <cassert>
#include <limits>
#include <functional>
#include "matrix.hpp"


// Helpers /////////////////////////////////////////////////////////////////////

enum class CellType {
	kVoid,
	kRoad,
	kStart,
	kEnd,
};

enum class Direction {
	kUp,
	kRight,
	kDown,
	kLeft,
	//
	kNone,
};

enum class CarStatus {
	kInGarage,
	kDriving,
	kArrived,
};


struct Position {
	int row = 0;
	int col = 0;
};

struct Route {
	int distance = std::numeric_limits<int>::max();
	Direction next = Direction::kNone;
};

struct Car {
	Car() = default;
	Car(int index, int target, int emission, int origin)
		: index(index)
		, target(target)
		, emission(emission)
		, origin(origin)
	{}

	// immutable
	int index = -1;
	int target = -1;
	int emission = -1;
	int origin = -1;

	// status
	Position pos;
	CarStatus status = CarStatus::kInGarage;

	// routing data
	Direction next = Direction::kNone;
	int prio = 0;
	int color = 0;
};

struct Cell {
	CellType type = CellType::kVoid;
	bool has_neighbor[4] = {};
	Route route[9];
	Car* current = nullptr;
	std::array<Car*, 4> candidates;
};

struct Garage {
	Position pos;
	std::deque<Car*> cars;
};

int fromDirection(Direction dir) {
	int d = int(dir);
	assert(d >= 0 && d < 4);
	return d;
}

Direction toDirection(int d) {
	assert(d >= 0 && d < 4);
	return Direction(d);
}

char toCommand(Direction dir) {
	switch (dir) {
		case Direction::kUp: return '^';
		case Direction::kRight: return '>';
		case Direction::kDown: return 'v';
		case Direction::kLeft: return '<';
		case Direction::kNone: return '-';
	}
	return '-';
}

Direction opposite(Direction dir) {
	switch (dir) {
		case Direction::kUp: return Direction::kDown;
		case Direction::kRight: return Direction::kLeft;
		case Direction::kDown: return Direction::kUp;
		case Direction::kLeft: return Direction::kRight;
		case Direction::kNone: return Direction::kNone;
	}
	return Direction::kNone;
}

Position neighbor(const Position& pos, Direction dir) {
	switch (dir) {
		case Direction::kUp: return {pos.row-1, pos.col};
		case Direction::kRight: return {pos.row, pos.col+1};
		case Direction::kDown: return {pos.row+1, pos.col};
		case Direction::kLeft: return {pos.row, pos.col-1};
		case Direction::kNone: return pos;
	}
	return {};
}

template<typename T>
std::vector<T> concat(const std::vector<T>& lhs, const std::vector<T>& rhs) {
	auto result = lhs;
	result.insert(result.end(), rhs.begin(), rhs.end());
	return result;
}

std::ostream& operator<<(std::ostream& stream, const Position& pos) {
	stream << "(" << pos.row << ", " << pos.col << ")";
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const Car& car) {
	stream << "<Car #" << car.index << " " << car.pos << ">";
	return stream;
}

bool operator==(const Position& lhs, const Position& rhs) {
	return lhs.row == rhs.row && lhs.col == rhs.col;
}

bool operator!=(const Position& lhs, const Position& rhs) {
	return !(lhs == rhs);
}

bool operator<(const Position& lhs, const Position& rhs) {
	return std::tie(lhs.row, lhs.col) < std::tie(rhs.row, rhs.col);
}


CellType cellTypeFromChar(char ch) {
	switch (ch) {
		case '*': return CellType::kRoad;
		case 'G': return CellType::kStart;
		case 'W': return CellType::kEnd;
		default: return CellType::kVoid;
	}
}

char cellTypeToChar(CellType type) {
	switch (type) {
		case CellType::kRoad: return '*';
		case CellType::kStart: return 'G';
		case CellType::kEnd: return 'W';
		case CellType::kVoid: return '-';
	}
	return ' ';
}

// City ////////////////////////////////////////////////////////////////////////

class City {
public:
	void fromStream(std::istream& stream);
	void showRoute(int start, int target);
	void solve();

private:
	void createGraph();
	void createRoute(int target);
	void foreachNeighbor(const Position& pos,
		std::function<void(const Position&, Direction)> fn);
	bool isValid(const Position& pos);
	Cell& getCell(const Position& pos);
	Route& getRoute(const Position& pos, int target);
	Car*& getCandidate(const Position& pos, Direction dir);
	const Route& getCarRoute(const Car& car);
	Position getCarNextPos(const Car& car);
	std::vector<Car*> garageFirsts();
	int countCandidates(const Position& pos);

	int size_ = 0;
	int targets_ = 0;
	matrix<Cell> cells_;
	std::vector<Position> target_pos_;
	std::vector<Car> cars_;
	std::vector<Garage> garages_;
};


void City::fromStream(std::istream& stream) {
	stream >> size_;
	cells_ = matrix<Cell>(size_, size_);
	for (int row = 0; row < size_; ++row) {
		std::string line;
		stream >> line;
		assert(line.size() >= size_);

		for (int col = 0; col < size_; ++col) {
			auto type = cellTypeFromChar(line[col]);
			cells_(row, col).type = type;
			if (type == CellType::kStart) {
				// Position pos{row, col};
				garages_.push_back({Position{row, col}});
			} else if (type == CellType::kEnd) {
				target_pos_.push_back({row, col});
			}
		}
	}
	targets_ = target_pos_.size();

	int k = 0;
	stream >> k;
	cars_.resize(k);
	for (int i = 0; i < k; ++i) {
		int g, w, e;
		stream >> g >> w >> e;
		cars_[i] = Car(i, w, e, g);
		cars_[i].pos = garages_[g].pos;
		garages_[g].cars.push_back(&cars_[i]);
	}

	createGraph();
	for (int i = 0; i < targets_; ++i) {
		createRoute(i);
	}
}

bool City::isValid(const Position& pos) {
	return
		pos.row >= 0 && pos.row < size_ &&
		pos.col >= 0 && pos.col < size_;
}

Cell& City::getCell(const Position& pos) {
	return cells_(pos.row, pos.col);
}

Route& City::getRoute(const Position& pos, int target) {
	return getCell(pos).route[target];
}

Car*& City::getCandidate(const Position& pos, Direction dir) {
	return getCell(pos).candidates[fromDirection(dir)];
}

const Route& City::getCarRoute(const Car& car) {
	return getRoute(car.pos, car.target);
}

Position City::getCarNextPos(const Car& car) {
	return neighbor(car.pos, car.next);
}

void City::foreachNeighbor(
	const Position& pos,
	std::function<void(const Position&, Direction dir)> fn)
{
	auto& cell = getCell(pos);
	for (int i = 0; i < 4; ++i) {
		if (cell.has_neighbor[i]) {
			auto dir = toDirection(i);
			fn(neighbor(pos, dir), dir);
		}
	}
}

void City::createGraph() {
	for (int row = 0; row < size_; ++row) {
		for (int col = 0; col < size_; ++col) {
			auto& cell = getCell({row, col});
			if (cell.type == CellType::kVoid) {
				continue;
			}
			for (int i = 0; i < 4; ++i) {
				Position nb = neighbor({row, col}, toDirection(i));
				if (!isValid(nb)) {
					continue;
				}
				if (getCell(nb).type == CellType::kRoad) {
					cell.has_neighbor[i] = true;
				}
			}
		}
	}
}

void City::createRoute(int target) {
	std::deque<Position> pipe;
	auto& root_pos = target_pos_[target];
	getRoute(root_pos, target) = Route{0, Direction::kNone};
	pipe.push_back(root_pos);

	while (!pipe.empty()) {
		auto pos = pipe.front();
		auto& cell = getCell(pos);
		auto nb_distance = cell.route[target].distance + 1;

		pipe.pop_front();
		foreachNeighbor(pos, [&](const Position& nb_pos, Direction dir) {
			auto& nb_cell = getCell(nb_pos);
			auto& route = nb_cell.route[target];
			if (route.next != Direction::kNone) {
				return;
			}
			route = Route{nb_distance, opposite(dir)};
			pipe.push_back(nb_pos);
		});
	}

	for (auto& garage : garages_) {
		auto sp = garage.pos;
		auto dst = std::numeric_limits<int>::max();
		auto next = Direction::kNone;
		foreachNeighbor(sp, [&](const Position& nb_pos, Direction dir) {
			auto nb_dst = getRoute(nb_pos, target).distance;
			if (nb_dst < dst) {
				next = dir;
				dst = nb_dst;
			}
		});
		getRoute(sp, target) = Route{dst, next};
	}
}

void City::showRoute(int start, int target) {
	auto sp = garages_[start].pos;
	auto next = getRoute(sp, target).next;

	std::cerr << sp << std::endl;

	std::vector<std::string> lines;
	lines.resize(size_);
	for (int row = 0; row < size_; ++row) {
		auto& line = lines[row];
		for (int col = 0; col < size_; ++col) {
			auto ch = cellTypeToChar(getCell({row, col}).type);
			line += ch;
		}
	}

	while (next != Direction::kNone) {
		sp = neighbor(sp, next);
		next = getRoute(sp, target).next;
		if (next != Direction::kNone) {
			lines[sp.row][sp.col] = '@';
		}
	}

	for (auto& line : lines) {
		std::cerr << line << std::endl;
	}
}

std::vector<Car*> City::garageFirsts() {
	std::vector<Car*> result;
	for (auto& garage : garages_) {
		if (garage.cars.empty()) {
			continue;
		}
		result.push_back(garage.cars.front());
	}
	return result;
}

int City::countCandidates(const Position& pos) {
	auto& cell = getCell(pos);
	int count = 0;
	for (int i = 0; i < 4; ++i) {
		if (cell.candidates[i]) {
			++count;
		}
	}
	return count;
}

void City::solve() {
	using Commands = std::vector<std::pair<int, Direction>>;
	std::vector<Car*> driving;
	while (true) {
		Commands cmds;
		auto movable_cars = concat(driving, garageFirsts());

		// clear cells
		for (auto& cell : cells_) {
			cell.current = nullptr;
			cell.candidates.fill(nullptr);
		}

		// update cars and cells
		for (auto* car : movable_cars) {
			auto pos = car->pos;
			auto next = getCarRoute(*car).next;
			auto next_pos = neighbor(pos, next);

			getCell(pos).current = car;
			getCandidate(next_pos, opposite(next)) = car;

			car->prio = 0;
			car->color = 0;
			car->next = next;
		}

		// resolve circles
		{
			std::set<Car*> cars(movable_cars.begin(), movable_cars.end());
			while (!cars.empty()) {
				auto* car0 = *cars.begin();
				auto* car = car0;
				bool blocked = false;

				assert(car->color == 0);
				car->color = 1;
				while (true) {
					car = getCell(getCarNextPos(*car)).current;
					if (!car || car->color == 2) {
						// previous cars may still proceed here
						break;
					}

					if (car->color == 3) {
						// this is part of a blocked area, other cars
						// cannot go here
						blocked = true;
						break;
					}

					// circle
					if (car->color == 1) {
						// circles make blocked areas, but cars in there
						// should still move (prio > 0)
						blocked = true;
						while (car->color != 3) {
							auto ec = cars.erase(car);
							assert(ec == 1);
							car->color = 3;
							car->prio = 10;
							car = getCell(getCarNextPos(*car)).current;
						}
						break;
					} else {
						assert(car->color == 0);
						car->color = 1;
					}
				}

				car = car0;
				while (car->color == 1) {
					auto ec = cars.erase(car);
					assert(ec == 1);
					car->color = blocked ? 3 : 2;
					car->prio = blocked ? -1 : 0;
					car = getCell(getCarNextPos(*car)).current;
				}
			}
		}

		// resolve free conflicts
		{
			std::set<Position> steps;
			for (auto* car : movable_cars) {
				steps.insert(car->pos);
			}

			for (auto& pos : steps) {
				if (countCandidates(pos) > 1) {
					// calculate_trails()
				}
			}
		}

		// move unobstructed cars
		// - todo implement -

		// create commands
		for (auto* car : movable_cars) {
			if (car->prio == 0) {
				continue;
			}
			if (car->status == CarStatus::kInGarage) {
				car->status = CarStatus::kDriving;
				auto& garage_queue = garages_[car->origin].cars;
				assert(garage_queue.front() == car);
				garage_queue.pop_front();
				driving.push_back(car);
			}

			auto next = getRoute(car->pos, car->target).next;
			cmds.push_back({car->index, next});
		}

		// commit
		std::cout << cmds.size() << std::endl;
		for (auto& cmd : cmds) {
			auto index = cmd.first;
			auto next = cmd.second;
			auto& car = cars_[index];

			car.pos = neighbor(car.pos, next);
			if (car.pos == target_pos_[car.target]) {
				car.status = CarStatus::kArrived;
			}
			std::cout
				<< index << " " << toCommand(next)
				<< std::endl;
		}

		// cleanup
		driving.erase(std::remove_if(driving.begin(), driving.end(),
			[](Car* car) -> bool {
				return (car->status == CarStatus::kArrived);
			}), driving.end());

		if (cmds.size() == 0) {
			break;
		}
	}
	std::cout << 0 << std::endl;

#if 0
	for (auto& garage : garages_) {
		std::list<Car*> driving;
		while (!garage.empty() || !driving.empty()) {
			Commands cmds;

			// prepare
			for (auto* car : driving) {
				auto next = getRoute(car->pos, car->target).next;
				cmds.push_back({car->index, next});
			}

			if (!garage.empty()) {
				auto* car = garage.front();
				auto next = getRoute(car->pos, car->target).next;
				driving.push_back(car);
				garage.pop_front();
				cmds.push_back({car->index, next});
				car->status = CarStatus::kDriving;
			}

			// commit
			std::cout << cmds.size() << std::endl;
			for (auto& cmd : cmds) {
				auto index = cmd.first;
				auto next = cmd.second;
				auto& car = cars_[index];

				car.pos = neighbor(car.pos, next);
				if (car.pos == target_pos_[car.target]) {
					car.status = CarStatus::kArrived;
				}
				std::cout
					<< index << " " << toCommand(next)
					<< std::endl;
			}

			// cleanup
			driving.remove_if([](Car* car) -> bool {
				return (car->status == CarStatus::kArrived);
			});
		}
	}
	std::cout << 0 << std::endl;
#endif
}


// Main ////////////////////////////////////////////////////////////////////////

int main() {
	City city;
	city.fromStream(std::cin);
	city.solve();
	return 0;
}
