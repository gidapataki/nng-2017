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

	// algo
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

	void calculatePath(Car* car);

private:
	bool isOccupied(const Position& pos, int tick);
	bool isValid(const Position& pos);
	void createGraph();
	void calculateDistances();
	Cell& getCell(const Position& pos);
	int& getDistance(const Position& pos, int target);
	const std::vector<Neighbor>& getNeighbors(const Position& pos);
	void setOccupied(const Position& pos, int tick);
	Direction getDirection(const Position& lhs, const Position& rhs);
	void addMove(int tick, const Move& move);
	bool checkGarage(int line, int g);

	int size_ = 0;
	int targets_ = 0;
	matrix<Cell> cells_;
	std::vector<Position> target_pos_;
	std::vector<Car> cars_;
	std::vector<Garage> garages_;
	std::vector<std::vector<Move>> tick_moves_;
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
		cars_[i].pos = garages_[g].pos;
		garages_[g].cars.push_back(&cars_[i]);
	}
}

void City::addMove(int tick, const Move& move) {
	if (tick >= tick_moves_.size()) {
		tick_moves_.resize(tick + 1);
	}
	tick_moves_[tick].push_back(move);
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

bool City::isOccupied(const Position& pos, int tick) {
	auto& cell = getCell(pos);
	if (tick < cell.occupied.size()) {
		return cell.occupied[tick];
	}
	return false;
}

void City::setOccupied(const Position& pos, int tick) {
	auto& cell = getCell(pos);
	if (tick >= cell.occupied.size()) {
		cell.occupied.resize((tick + 1) * 1);
	}

	cell.occupied[tick] = true;
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

void City::calculatePath(Car* car) {
	Position pos0 = car->pos;
	int target = car->target;
	// Key = <distance, delta_tick, prev, dir, pos>
	using Key = std::tuple<int, int, int, Direction, Position>;
	using Queue = std::priority_queue<Key, std::vector<Key>, std::greater<Key>>;
	using Step = std::pair<int, Direction>;

	Queue queue;
	std::vector<Step> steps;
	auto tp = target_pos_[target];
	int tick0 = 0;

	while (isOccupied(pos0, tick0)) {
		++tick0;
	}

	int dst0 = getDistance(pos0, target);
	bool debug = false;
	bool found = false;

	while (!found) {
		queue = {};
		steps.clear();
		queue.push({dst0, 0, -1, Direction::kNone, pos0});

		while (!queue.empty()) {
			int tick = tick0;
			int dst, dtick, prev;
			Position pos;
			Direction dir;
			std::tie(dst, dtick, prev, dir, pos) = queue.top();
			queue.pop();
			tick += dtick;

			int current = steps.size();
			steps.push_back({prev, dir});

			if (dst == 1 && !isOccupied(tp, tick + 1)) {
				steps.push_back({current, getDirection(pos, tp)});
				found = true;
				break;
			}

			if (!isOccupied(pos, tick + 1)) {
				queue.push({dst, dtick + 1, current, Direction::kNone, pos});
			}

			for (auto nb : getNeighbors(pos)) {
				if (isOccupied(nb.pos, tick + 1)) {
					continue;
				}
				auto ndst = getDistance(nb.pos, target);
				queue.push({ndst, dtick + 1, current, nb.dir, nb.pos});
			}

			if (steps.size() > 1000) {
				break;
			}
		}

		if (!found) {
			setOccupied(pos0, tick0);
			++tick0;
		}
	}

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
			setOccupied(pos, tick);
			++tick;
		}
	}

#if 0
	for (int g = 0; g < garages_.size(); ++g) {
		if (!checkGarage(__LINE__, g)) {
			std::cerr << car->index << std::endl;
			std::cerr << car->origin << std::endl;
			std::cerr << car->pos << std::endl;
			std::cerr << garages_[g].pos << std::endl;
			assert(false);
		}
	}
#endif

	{
		int dticks = dirs.size();
		for (int d = 1; d < dticks; ++d) {
			auto dir = dirs[d];
			if (dir == Direction::kNone) {
				continue;
			}
			addMove(d + tick0 - 1, {car->index, dir});
		}
	}

#if 0
	std::cerr << "D " << car->index << " " << steps.size();
	for (auto dir : dirs) {
		std::cerr << " " << toCommand(dir);
	}
	std::cerr << std::endl;
#endif
}

bool City::checkGarage(int line, int g) {
	auto& occ = getCell(garages_[g].pos).occupied;
	auto x = true;
	for (int i = 0; i < occ.size(); ++i) {
		if (occ[i] && !x) {
			std::cerr << "ERROR(" << line << ") " << g << std::endl;
			return false;
		}
		x = occ[i];
	}
	return true;
}

void City::solve() {
	createGraph();
	calculateDistances();

	int count = 0;
	bool has_cars = true;
	while (has_cars) {
		has_cars = false;
		for (auto& garage : garages_) {
			if (garage.cars.empty()) {
				continue;
			}
			has_cars = true;
			auto* car = garage.cars.front();
			garage.cars.pop_front();

			++count;
			// std::cerr << count << std::endl;
			calculatePath(car);
		}
	}

	for (auto& moves : tick_moves_) {
		std::cout << moves.size() << std::endl;
		for (auto& move : moves) {
			std::cout << move.index << " " << toCommand(move.dir) << std::endl;
		}
	}
	std::cout << 0 << std::endl;
}

// Main ////////////////////////////////////////////////////////////////////////

int main() {
	City city;
	city.fromStream(std::cin);
	city.solve();
	return 0;
}
