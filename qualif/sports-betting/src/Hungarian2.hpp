#ifndef HUNGARIAN2_HPP_
#define HUNGARIAN2_HPP_

#include <algorithm>
#include <deque>
#include <iostream>
#include <map>
#include <numeric>
#include <type_traits>
#include <vector>

#include <boost/assert.hpp>
#include <boost/optional.hpp>

template<typename Value>
class Hungarian2 {
public:
	Hungarian2(std::vector<std::vector<Value>> m) :
			cost(std::move(m)),
			mask(cost.size(), std::vector<int>(cost.size(), 0)),
			rowCover(cost.size(), false),
			columnCover(cost.size(), false) {
		BOOST_ASSERT(cost.size() == cost.front().size());
		static_assert(std::is_same<decltype(k2), decltype(cost.size())>::value, "");
		k2 = cost.size();
	}

	std::vector<Value> getCosts() {
		auto step = 1;
		static constexpr std::array<std::size_t (Hungarian2::*)(), 7> steps {
			nullptr,
			&Hungarian2::step1,
			&Hungarian2::step2,
			&Hungarian2::step3,
			&Hungarian2::step4,
			&Hungarian2::step5,
			&Hungarian2::step6,
		};
		while (step != 0) {
			step = (this->*steps[step])();
		}
		std::vector<Value> result;
		result.reserve(cost.size());
		std::cerr << "asd" << cost.size() << std::endl;
		for (const auto& row: cost) {
			for (const auto& cell: row) {
				std::cerr << cell << " ";
			}
			std::cerr << std::endl;
		}
		for (const auto& row: cost) {
			for (const auto& cell: row) {
				if (cell != zero) {
					result.push_back(cell);
				}
			}
		}
		return result;
	}

private:
	std::size_t step1() {
		for (auto& row: cost) {
			auto smallestElement =
					*std::min_element(row.begin(), row.end());
			std::transform(row.begin(), row.end(), row.begin(),
					[&smallestElement](auto n) { return n-smallestElement; });
		}
		return 2;
	}

	std::size_t step2() {
		for (std::size_t rowIndex = 0; rowIndex < cost.size(); ++rowIndex) {
			for (std::size_t columnIndex = 0; columnIndex < cost.size();
					++columnIndex) {
				if (cost[rowIndex][columnIndex] == zero &&
						!rowCover[rowIndex] &&
						!columnCover[columnIndex]) {
					mask[rowIndex][columnIndex] = 1;
					rowCover[rowIndex] = true;
					columnCover[columnIndex] = true;
			    }
			}
		}
		clearCovers();
		return 3;
	}

	std::size_t step3() {
		for (const auto& maskRow: mask) {
			for (std::size_t i = 0; i < maskRow.size(); ++i) {
				if (maskRow[i] == 1) {
					columnCover[i] = true;
				}
			}
		}
		std::size_t columnCount =
				std::count(columnCover.begin(), columnCover.end(), true);
		if (columnCount >= cost.size()) {
			return 0;
		} else {
			return 4;
		}
	}

	std::size_t step4() {
		for(;;) {
			auto zeroPosition = findZero();
			if (!zeroPosition) {
				return 6;
			}
			auto row = zeroPosition->first;
			auto column = zeroPosition->second;
			mask[row][column] = 2;
			auto starPosition = findStarInRow(row);
			if (starPosition) {
				rowCover[row] = true;
				columnCover[*starPosition] = false;
			} else {
				pathRow0 = row;
				pathColumn0 = -1;
				return 5;
			}
		}
	}

	std::size_t step5() {
		paths.clear();
		paths.push_back(std::make_pair(pathRow0, pathColumn0));
		for(;;) {
			auto starPosition = findStarInColumn(paths.back().second);
			if (!starPosition) {
				break;
			}
			paths.push_back(std::make_pair(*starPosition, paths.back().second));
			auto primePosition = findPrimeInRow(paths.back().first);
			BOOST_ASSERT_MSG(primePosition, "c should not be -1");
			paths.push_back(std::make_pair(paths.back().first, *primePosition));
		}
		augmentPaths();
		clearCovers();
		erasePrimes();
		return 3;
	}

	std::size_t step6() {
		auto smallest = findSmallestUncovered();
		BOOST_ASSERT_MSG(smallest,
				"At this point, at least one value should be uncovered");
		for (std::size_t rowIndex = 0; rowIndex < cost.size(); ++rowIndex) {
			for (std::size_t columnIndex = 0;
					columnIndex < cost[rowIndex].size(); ++columnIndex) {
				if (rowCover[rowIndex]) {
					cost[rowIndex][columnIndex] += *smallest;
				}
				if (!columnCover[columnIndex]) {
					cost[rowIndex][columnIndex] -= *smallest;
				}
			}
		}
		return 4;
	}

	boost::optional<std::pair<std::size_t, std::size_t>> findZero() const {
		for (std::size_t rowIndex = 0; rowIndex < cost.size(); ++rowIndex) {
			const auto& row = cost[rowIndex];
			auto it = std::find(row.begin(), row.end(), zero);
			if (it != row.end()) {
				return std::make_pair(rowIndex,
						static_cast<std::size_t>(
								std::distance(row.begin(), it)));
			}
		}
		return boost::none;
	}

	boost::optional<std::size_t> findStarInRow(std::size_t rowIndex) const {
		const auto& maskRow = mask[rowIndex];
		auto it = std::find(maskRow.begin(), maskRow.end(), 1);
		if (it != maskRow.end()) {
			return std::distance(maskRow.begin(), it);
		}
		return boost::none;
	}

	boost::optional<std::size_t> findStarInColumn(
			std::size_t columnIndex) const {
		for (std::size_t rowIndex = 0; rowIndex < mask.size(); ++rowIndex) {
			if (mask[rowIndex][columnIndex] == 1) {
				return rowIndex;
			}
		}
		return boost::none;
	}

	boost::optional<std::size_t> findPrimeInRow(std::size_t rowIndex) const {
		const auto& maskRow = mask[rowIndex];
		auto it = std::find(maskRow.begin(), maskRow.end(), 2);
		if (it != maskRow.end()) {
			return std::distance(maskRow.begin(), it);
		}
		return boost::none;
	}

	void augmentPaths() {
		for (const auto& path: paths) {
			if (mask[path.first][path.second] == 1) {
				mask[path.first][path.second] = 0;
			} else {
				mask[path.first][path.second] = 1;
			}
		}
	}

	void clearCovers() {
		set_all_values_to(rowCover, false);
		set_all_values_to(columnCover, false);
	}

	void erasePrimes() {
		for (auto& maskRow: mask) {
			for (auto& maskCell: maskRow) {
				if (maskCell == 2) {
					maskCell = 0;
				}
			}
		}
	}

	boost::optional<Value> findSmallestUncovered() const {
		boost::optional<Value> result = boost::none;
		for (std::size_t rowIndex = 0; rowIndex < cost.size(); ++rowIndex) {
			for (std::size_t columnIndex = 0;
					columnIndex < cost[rowIndex].size(); ++columnIndex) {
				if (!rowCover[rowIndex] && !columnCover[columnIndex]) {
					const auto& candidateValue = cost[rowIndex][columnIndex];
					if (!result || (candidateValue < *result)) {
						result = candidateValue;
					}
				}
			}
		}
		return result;
	}

	template <typename Range>
	static void set_all_values_to(Range& r,
			const typename Range::value_type& value) {
		for (auto& v: r) {
			v = value;
		}
	}

	const Value zero = Value{};


	std::vector<std::vector<Value>> cost;
	std::vector<std::vector<int>> mask;
	std::deque<bool> rowCover;
	std::deque<bool> columnCover;
	std::size_t k2 = 0;
	std::size_t pathRow0 = 0;
	std::size_t pathColumn0 = 0;
	std::vector<std::pair<std::size_t, std::size_t>> paths;

};

#endif // HUNGARIAN2_HPP_
