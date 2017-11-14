#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <queue>
#include <set>
#include <array>
#include <cassert>
#include <limits>
#include <functional>
#include <algorithm>
#include <chrono>

// matrix.hpp

template<class T>
class matrix {
public:
    typedef T               value_type;
    typedef T*              pointer;
    typedef const T*        const_pointer;
    typedef T&              reference;
    typedef const T&        const_reference;
    typedef T*              iterator;
    typedef const T*        const_iterator;
    typedef std::size_t     size_type;
    typedef std::ptrdiff_t  difference_type;
public:
    matrix();
    matrix(const matrix& o);
    matrix(size_type width, size_type height, const T& init_value = T());

    matrix& operator=(const matrix& o);

    virtual ~matrix();

    bool operator==(const matrix& o) const;
    bool operator!=(const matrix& o) const;

    reference at(size_type x, size_type y);
    const_reference at(size_type x, size_type y) const;

    reference operator()(size_type x, size_type y);
    const_reference operator()(size_type x, size_type y) const;

    iterator begin();
    iterator end();

    const_iterator begin() const;
    const_iterator end() const;

    size_type width() const;
    size_type height() const;

    void swap(matrix<T>& rhs);
protected:
    size_type width_;
    size_type height_;
    T *data_;
};

template<class T>
matrix<T>::matrix() : width_(0), height_(0), data_(0) {}

template<class T>
matrix<T>::matrix(const matrix<T>& o) : width_(o.width_), height_(o.height_) {
    if ( o.data_ ) {
        data_ = new T[width_ * height_];
        std::copy( o.begin(), o.end(), begin() );
    } else {
        data_ = 0;
    }
}

template<class T>
matrix<T>::matrix(size_type width, size_type height, const T& init_value) :
    width_(width),
    height_(height) {
    if ( width_ == 0 || height_ == 0 ) {
        data_ = 0;
    } else {
        data_ = new T[width_ * height_];
        std::fill(data_, data_ + (width_*height_), init_value);
    }
}

template<class T>
matrix<T>& matrix<T>::operator=(const matrix<T>& o) {
    if ( this == &o ) {
        return *this;
    }

    delete[] data_;

    width_ = o.width_;
    height_ = o.height_;
    if ( o.data_ ) {
        data_ = new T[width_ * height_];
        std::copy( o.begin(), o.end(), begin() );
    } else {
        data_ = 0;
    }
    return *this;
}

template<class T>
matrix<T>::~matrix() {
    delete[] data_;
}

template<class T> inline
bool matrix<T>::operator==(const matrix<T>& o) const {
    return width_ == o.width_ && height_ == o.height && std::equal( data_, data_ + width_*height_, o.data_);
}

template<class T> inline
bool matrix<T>::operator!=(const matrix<T>& o) const {
    return !(*this == o);
}

template<class T> inline
typename matrix<T>::reference matrix<T>::at(size_type x, size_type y) {
    if ( x >= width_ || y >= height_ ) {
        throw std::out_of_range("matrix : Out of range");

    }
    return this->operator()(x,y);
}

template<class T> inline
typename matrix<T>::const_reference matrix<T>::at(size_type x, size_type y) const {
    if ( x >= width_ || y >= height_ ) {
        throw std::out_of_range("matrix : Out of range");
    }
    return this->operator()(x,y);
}

template<class T> inline
typename matrix<T>::reference matrix<T>::operator()(size_type x, size_type y) {
    return data_[y*width_+x];
}

template<class T> inline
typename matrix<T>::const_reference matrix<T>::operator()(size_type x, size_type y) const {
    return data_[y*width_+x];
}

template<class T> inline
typename matrix<T>::iterator matrix<T>::begin() {
    return data_;
}

template<class T> inline
typename matrix<T>::iterator matrix<T>::end() {
    return data_ + (width_*height_);
}

template<class T> inline
typename matrix<T>::const_iterator matrix<T>::begin() const {
    return data_;
}

template<class T> inline
typename matrix<T>::const_iterator matrix<T>::end() const {
    return data_ + (width_*height_);
}

template<class T> inline
typename matrix<T>::size_type matrix<T>::width() const {
    return width_;
}

template<class T> inline
typename matrix<T>::size_type matrix<T>::height() const {
    return height_;
}

template<class T> inline
void matrix<T>::swap(matrix<T>& rhs) {
    std::swap(width_, rhs.width_);
    std::swap(height_, rhs.height_);
    std::swap(data_, rhs.data_);
}


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
	int dst = std::numeric_limits<int>::max();
};

struct Neighbor {
	// Note: pos == neighbor(pos0, dir)
	// 		 pos0 == neighbor(pos, opposite(dir))
	Position pos;
	Direction dir = Direction::kNone;
};

struct Cell {
	// immutable
	std::vector<Neighbor> neighbors;
	CellType type = CellType::kVoid;
	int distance[9] = {};
	bool has_neighbor[4] = {};
};

struct CellData {
	std::vector<bool> occupied;
};

struct Garage {
	Position pos;
	std::deque<Car*> cars;
};

struct Move {
	int index = 0;
	Direction dir = Direction::kNone;
};

struct Route {
	int dst = 0;
	int delta_tick = 0;
	int prev = 0;

	Direction dir;
	Position pos;
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
		case Direction::kNone: return '.';
	}
	return '.';
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
	return pos;
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

bool operator<=(const Position& lhs, const Position& rhs) {
	return std::tie(lhs.row, lhs.col) <= std::tie(rhs.row, rhs.col);
}

bool operator>(const Position& lhs, const Position& rhs) {
	return std::tie(lhs.row, lhs.col) > std::tie(rhs.row, rhs.col);
}

bool operator>=(const Position& lhs, const Position& rhs) {
	return std::tie(lhs.row, lhs.col) >= std::tie(rhs.row, rhs.col);
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
	void solve();

private:
	using RouteCompare = std::function<bool(const Route&, const Route&)>;

	struct Solution {
		matrix<CellData> data;
		std::vector<std::vector<Move>> tick_moves;
		int sum_emission = 0;
	};

	struct Args {
		int steps_limit = 0;
		int color_limit = 0;
		double tick_limit = 0;
		RouteCompare compare;
	};

	void initSolution(Solution& sol);
	void solve(const Args& args, Solution& sol);
	void improveSolution(const Args& args);

	void setOccupied(Solution& sol, const Position& pos, int tick);
	bool isOccupied(Solution& sol, const Position& pos, int tick);
	void addMove(Solution& sol, int tick, const Move& move);
	void addEmission(Solution& sol, int emission);
	void calculatePath(Solution& sol, const Args& args, Car* car);

	bool isValid(const Position& pos);
	void createGraph();
	void calculateDistances();
	Cell& getCell(const Position& pos);
	int& getDistance(const Position& pos, int target);
	const std::vector<Neighbor>& getNeighbors(const Position& pos);
	Direction getDirection(const Position& lhs, const Position& rhs);
	void prepareCars();

	int size_ = 0;
	int targets_ = 0;
	matrix<Cell> cells_;
	std::vector<Position> target_pos_;
	std::vector<Car> cars_;
	std::vector<Garage> garages_;

	int sol_count_ = 0;
	Solution solution_;
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
		garages_[g].cars.push_back(&cars_[i]);
	}
}

void City::prepareCars() {
	for (auto& car : cars_) {
		car.pos = garages_[car.origin].pos;
		car.dst = getDistance(car.pos, car.target);
	}
}

void City::addMove(Solution& sol, int tick, const Move& move) {
	if (tick >= sol.tick_moves.size()) {
		sol.tick_moves.resize(tick + 1);
	}
	sol.tick_moves[tick].push_back(move);
}

void City::addEmission(Solution& sol, int emission) {
	sol.sum_emission += emission;
}

Direction City::getDirection(const Position& lhs, const Position& rhs) {
	if (lhs == rhs) {
		return Direction::kNone;
	}

	if (lhs.row == rhs.row) {
		if (lhs.col + 1 == rhs.col) {
			return Direction::kRight;
		}
		if (lhs.col - 1 == rhs.col) {
			return Direction::kLeft;
		}
	}
	if (lhs.col == rhs.col) {
		if (lhs.row + 1 == rhs.row) {
			return Direction::kDown;
		}
		if (lhs.row - 1 == rhs.row) {
			return Direction::kUp;
		}
	}

	assert(false && "Invalid neighbor");
	return Direction::kNone;
}

Cell& City::getCell(const Position& pos) {
	return cells_(pos.row, pos.col);
}

int& City::getDistance(const Position& pos, int target) {
	return getCell(pos).distance[target];
}

const std::vector<Neighbor>& City::getNeighbors(const Position& pos) {
	return getCell(pos).neighbors;
}

bool City::isValid(const Position& pos) {
	return
		pos.row >= 0 && pos.row < size_ &&
		pos.col >= 0 && pos.col < size_;
}

void City::initSolution(Solution& sol) {
	sol.data = matrix<CellData>(size_, size_);
}

bool City::isOccupied(Solution& sol, const Position& pos, int tick) {
	auto& occupied_vec = sol.data(pos.row, pos.col).occupied;
	if (tick < occupied_vec.size()) {
		return occupied_vec[tick];
	}
	return false;
}

void City::setOccupied(Solution& sol, const Position& pos, int tick) {
	auto& occupied_vec = sol.data(pos.row, pos.col).occupied;
	if (tick >= occupied_vec.size()) {
		occupied_vec.resize((tick + 1) * 1);
	}

	occupied_vec[tick] = true;
}

void City::createGraph() {
	for (int row = 0; row < size_; ++row) {
		for (int col = 0; col < size_; ++col) {
			Position pos{row, col};
			auto& cell = getCell(pos);
			if (cell.type == CellType::kVoid) {
				continue;
			}
			for (int i = 0; i < 4; ++i) {
				auto dir = toDirection(i);
				Position nb = neighbor(pos, dir);
				if (!isValid(nb)) {
					continue;
				}
				if (getCell(nb).type == CellType::kRoad) {
					cell.has_neighbor[i] = true;
					cell.neighbors.push_back({nb, dir});
				}
			}
		}
	}
}

void City::calculateDistances() {
	for (auto& cell : cells_) {
		for (int target = 0; target < targets_; ++target) {
			cell.distance[target] = std::numeric_limits<int>::max();
		}
	}

	for (int target = 0; target < targets_; ++target) {
		auto tp = target_pos_[target];

		std::deque<Position> pipe;
		pipe.push_back(tp);
		getDistance(tp, target) = 0;

		while (!pipe.empty()) {
			auto pos = pipe.front();
			auto dst = getDistance(pos, target) + 1;
			pipe.pop_front();
			for (auto nb : getNeighbors(pos)) {
				auto& ndst = getDistance(nb.pos, target);
				if (ndst > dst) {
					ndst = dst;
					pipe.push_back(nb.pos);
				}
			}
		}

		for (auto& garage : garages_) {
			auto& gdst = getDistance(garage.pos, target);
			for (auto& nb : getNeighbors(garage.pos)) {
				auto dst = getDistance(nb.pos, target) + 1;
				gdst = std::min(gdst, dst);
			}
		}
	}
}


bool convergeRoutes(const Route& lhs, const Route& rhs) {
	auto lw0 = lhs.dst / 2;
	auto rw0 = rhs.dst / 2;
	auto lw1 = lhs.prev;
	auto rw1 = rhs.prev;
	auto lw2 = lhs.delta_tick;
	auto rw2 = rhs.delta_tick;

	return
		std::tie(lw0, lw2, lw1, lhs.dir, lhs.pos) >
		std::tie(rw0, lw2, rw1, rhs.dir, rhs.pos);
}

bool smallestTick(const Route& lhs, const Route& rhs) {
	return
		std::tie(lhs.delta_tick, lhs.dst) >
		std::tie(rhs.delta_tick, rhs.dst);
}

void City::calculatePath(Solution& sol, const Args& args, Car* car) {
	Position pos0 = car->pos;
	int target = car->target;

	using Key = Route;
	using KeyCompare = RouteCompare;
	using Queue = std::priority_queue<Key, std::vector<Key>, KeyCompare>;
	using Step = std::pair<int, Direction>;

	Queue queue;
	std::vector<Step> steps;

	auto tp = target_pos_[target];
	int tick0 = 0;

	while (isOccupied(sol, pos0, tick0)) {
		++tick0;
	}

	int failed = 0;
	int dst0 = getDistance(pos0, target);
	bool debug = false;
	bool found = false;

	int color_limit = args.color_limit;
	while (!found) {
		queue = Queue(args.compare);
		steps.clear();
		queue.push({dst0, 0, -1, Direction::kNone, pos0});
		matrix<int> colors(size_, size_);

		while (!queue.empty()) {
			auto route = queue.top();
			auto dst = route.dst;
			auto dtick = route.delta_tick;
			auto prev = route.prev;
			auto pos = route.pos;
			auto dir = route.dir;
			auto tick = tick0 + route.delta_tick;
			queue.pop();

			if ((colors(pos.row, pos.col) += 1) > color_limit) {
				continue;
			}

			int current = steps.size();
			steps.push_back({prev, dir});
			// std::cerr << current << pos << "  " << toCommand(dir) << std::endl;

			if (dst == 1 && !isOccupied(sol, tp, tick + 1)) {
				steps.push_back({current, getDirection(pos, tp)});
				found = true;
				break;
			}

			if (steps.size() > args.steps_limit) {
				break;
			}
#if 0
			if (args.tick_limit > 0 && dtick > 4 + args.tick_limit * dst) {
				break;
			}
#endif
			if (!isOccupied(sol, pos, tick + 1)) {
				queue.push({
					dst, dtick + 1,
					current, Direction::kNone, pos});
			}

			for (auto nb : getNeighbors(pos)) {
				if (isOccupied(sol, nb.pos, tick + 1)) {
					continue;
				}
				auto ndst = getDistance(nb.pos, target);
				queue.push({
					ndst, dtick + 1,
					current, nb.dir, nb.pos});
			}
		}

		if (!found) {
			setOccupied(sol, pos0, tick0);
			++tick0;
			++failed;
		}
	}

	assert(found && "Invalid state");

	std::vector<Direction> dirs;
	{
		auto pos = pos0;
		auto tick = tick0;
		{
			auto step = steps.back();
			while (true) {
				dirs.push_back(step.second);
				if (step.first == -1) {
					break;
				}
				step = steps[step.first];
			}
			std::reverse(dirs.begin(), dirs.end());
		}

		for (auto dir : dirs) {
			pos = neighbor(pos, dir);
			setOccupied(sol, pos, tick);
			++tick;
		}
	}

	{
		int dticks = dirs.size();
		for (int d = 1; d < dticks; ++d) {
			auto dir = dirs[d];
			if (dir == Direction::kNone) {
				continue;
			}
			addMove(sol, d + tick0 - 1, {car->index, dir});
		}

		addEmission(sol, (tick0 + dticks - 1) * car->emission);
	}

#if 0
	std::cerr << "D " << car->index << " " << steps.size();
	for (auto dir : dirs) {
		std::cerr << " " << toCommand(dir);
	}
	std::cerr << std::endl;
#endif
}

void City::solve(const Args& args, Solution& sol) {
	int count = 0;
	auto garages = garages_;

	while (true) {
		std::vector<Car*> cars;

		for (auto& garage : garages) {
			if (!garage.cars.empty()) {
				cars.push_back(garage.cars.front());
			}
		}

		if (cars.empty()) {
			break;
		}

		std::sort(cars.begin(), cars.end(), [](Car* lhs, Car* rhs) {
			auto l1 = -lhs->emission;
			auto r1 = -rhs->emission;
			return
				std::tie(l1) <
				std::tie(r1);
		});

		for (auto* car : cars) {
			auto& garage = garages[car->origin];
			calculatePath(sol, args, car);
			garage.cars.pop_front();
			++count;
			// std::cerr << "C " << count << std::endl;
		}
	}
}

void City::improveSolution(const Args& args) {
	Solution sol;
	initSolution(sol);
	solve(args, sol);

	if (sol_count_ == 0 || sol.sum_emission < solution_.sum_emission) {
		std::swap(sol, solution_);
	}
	++sol_count_;
}

void City::solve() {
    auto t0 = std::chrono::high_resolution_clock::now();
	createGraph();
	calculateDistances();
	prepareCars();

    auto t1 = std::chrono::high_resolution_clock::now();
    auto delta_t = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);

#if 1
	{
		Args args;
		args.steps_limit = 990;
		args.color_limit = 23;
		args.compare = convergeRoutes;
		improveSolution(args);
	}
#endif

#if 1
	{
		Args args;
		args.steps_limit = 1600;
		args.color_limit = 12;
		args.compare = convergeRoutes;
		improveSolution(args);
	}
#endif

#if 1
	t1 = std::chrono::high_resolution_clock::now();
	delta_t = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
	if (delta_t.count() < 5500) {
		Args args;
		args.steps_limit = 2100;
		args.color_limit = 10;
		args.compare = convergeRoutes;
		improveSolution(args);
	}
#endif


	if (sol_count_ > 0) {
		for (auto& moves : solution_.tick_moves) {
			std::cout << moves.size() << std::endl;
			for (auto& move : moves) {
				std::cout << move.index << " " << toCommand(move.dir) << std::endl;
			}
		}
	}
	std::cout << 0 << std::endl;

#if LOCAL_BUILD
	t1 = std::chrono::high_resolution_clock::now();
	delta_t = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
	std::cerr << "E " << solution_.sum_emission << std::endl;
    std::cerr << "Elapsed " << delta_t.count() << std::endl;
#endif
}

// Main ////////////////////////////////////////////////////////////////////////

int main() {
	City city;
	city.fromStream(std::cin);
	city.solve();
	return 0;
}
