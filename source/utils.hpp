#ifndef UTILS_HPP
#define UTILS_HPP
#include <utility>
#include <array>
#include <chrono>

#include "simple/support/range.hpp"
#include "simple/geom/vector.hpp"

using simple::support::range;
using bool2 = simple::geom::vector<bool,2>;
using int2 = simple::geom::vector<int,2>;

class frametime_logger
{
	using clock = std::chrono::steady_clock;
	clock::duration accum;
	size_t count;
	size_t limit;
	public:
	frametime_logger(size_t limit = 60);
	void log(clock::duration);
};

template <typename M, typename T, size_t N = 2>
// mask is a boolean, T is integral, both can be vectors
std::array<range<T>,N> split(const range<T> whole, M mask)
{
	std::array<range<T>, N> pieces{};
	T size = whole.upper() - whole.lower();
	auto piece_size = T{size / N};
	auto remainder = T{size % N};

	auto lower = whole.lower();
	for(size_t i = 0; i < N; ++i)
	{
		auto& piece = pieces[i];
		piece = range<T>{lower, lower + piece_size + (remainder --> T{})};
		lower = piece.upper();

		// apply mask
		for(size_t j = 0; j < piece.bounds.size(); ++j)
		{
			piece.bounds[j] *= mask;
			piece.bounds[j] += whole.bounds[j] * ~mask;
		}
	}

	return pieces;
}

template <size_t... Sizes>
constexpr auto prepare_splits(std::integer_sequence<size_t,Sizes...>)
{
	return std::make_tuple(&split<bool2,int2,Sizes>...);
}

template <typename V, typename T, T... Is>
constexpr auto upper_bound(std::integer_sequence<T,Is...>, const V& value)
{
	struct
	{
		T value;
		size_t index;
	} ret{};
	T arr[] = {Is...};
	auto lower = std::upper_bound(std::begin(arr), std::end(arr), value);
	lower = lower == std::begin(arr) ? lower : lower-1;
	ret.index = lower - std::begin(arr);
	ret.value = *lower;
	return ret;
}

#endif /* end of include guard */
