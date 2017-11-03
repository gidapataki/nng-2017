#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <list>
#include <cassert>
#include <limits>
#include <functional>


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

struct Cell {
	CellType type = CellType::kVoid;
	bool has_neighbor[4] = {};
	Route route[9];
};

struct Car {
	int index = -1;
	int target = -1;
	int emission = -1;
	Position pos;
	CarStatus status = CarStatus::kInGarage;
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

// Grid ////////////////////////////////////////////////////////////////////////

class Grid {
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

	int size_ = 0;
	int targets_ = 0;
	std::vector<std::vector<Cell>> cells_;
	std::vector<Position> start_pos_;
	std::vector<Position> end_pos_;
	std::vector<Car> cars_;
	std::vector<std::deque<Car*>> garages_;
};


void Grid::fromStream(std::istream& stream) {
	stream >> size_;
	cells_.resize(size_);
	for (int row = 0; row < size_; ++row) {
		std::string line;
		stream >> line;
		assert(line.size() >= size_);

		cells_[row].resize(size_);
		for (int col = 0; col < size_; ++col) {
			auto type = cellTypeFromChar(line[col]);
			cells_[row][col].type = type;
			if (type == CellType::kStart) {
				start_pos_.push_back({row, col});
			} else if (type == CellType::kEnd) {
				end_pos_.push_back({row, col});
			}
		}
	}
	targets_ = end_pos_.size();

	int k = 0;
	stream >> k;
	cars_.resize(k);
	garages_.resize(start_pos_.size());
	for (int i = 0; i < k; ++i) {
		int g, w, e;
		stream >> g >> w >> e;
		cars_[i] = Car{i, w, e, start_pos_[g], CarStatus::kInGarage};
		garages_[g].push_back(&cars_[i]);
	}

	createGraph();
	for (int i = 0; i < targets_; ++i) {
		createRoute(i);
	}
}

bool Grid::isValid(const Position& pos) {
	return
		pos.row >= 0 && pos.row < size_ &&
		pos.col >= 0 && pos.col < size_;
}

Cell& Grid::getCell(const Position& pos) {
	return cells_[pos.row][pos.col];
}

Route& Grid::getRoute(const Position& pos, int target) {
	return getCell(pos).route[target];
}

void Grid::foreachNeighbor(
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

void Grid::createGraph() {
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

void Grid::createRoute(int target) {
	std::deque<Position> pipe;
	auto& root_pos = end_pos_[target];
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

	for (auto& sp : start_pos_) {
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

void Grid::showRoute(int start, int target) {
	auto sp = start_pos_[start];
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

void Grid::solve() {
	using Commands = std::vector<std::pair<int, Direction>>;
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
				if (car.pos == end_pos_[car.target]) {
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
}


// Main ////////////////////////////////////////////////////////////////////////

int main() {
	Grid g;
	g.fromStream(std::cin);
	g.solve();
	return 0;
}
