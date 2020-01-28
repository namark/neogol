#ifndef FACTORS_HPP
#define FACTORS_HPP

#include <array>
#include <numeric>
#include <algorithm>

#include "simple/support/algorithm.hpp"

constexpr auto some_primes = std::array
{
	2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53,
	59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113,
	127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181,
	191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251,
	257, 263, 269, 271
};

// what is integer exp that you don't have it, eh? c++, eh? -_-
// TODO: add integer exp to simple::support i guess
template <typename Int, typename Power = Int>
constexpr auto pow(const Int& i, Power p)
	-> decltype((Int{}, ++i, i*=i, p-->0, i))
{
	auto r = Int{}; ++r; // ugh
	while(p-->0) r *= i;
	return r;
}

template <typename Prime, typename Power = Prime>
struct prime_factorization
{
	std::vector<Prime> primes;
	std::vector<Power> powers;
};

template <typename Int, typename Power = Int>
prime_factorization<Int, Power> partial_prime_factorize(Int number)
{
	prime_factorization<Int,Power> result{};
	for(auto&& prime : some_primes)
	{
		if(number % prime == 0)
		{
			result.primes.push_back(prime);
			result.powers.push_back(Power{});
			do
			{
				number /= prime;
				++result.powers.back();
			}
			while(number % prime == 0);
		}

		if(number == 1)
		{
			return result;
		}
	}

	result.primes.push_back(number);
	result.powers.push_back(Power());
	++result.powers.back();
	return result;
}

template <typename T>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

template <typename PrimeFactors, typename FactorPowers, typename Counter, typename F>
constexpr void for_all_factors(const PrimeFactors& primes, const FactorPowers& powers, Counter&& counter, F&& f)
{
	using simple::support::next_number;
	using simple::support::advance_vector;
	using std::begin, std::end;
	using prime_type = remove_cvref_t<decltype(*begin(primes))>;
	using power_type = remove_cvref_t<decltype(*begin(powers))>;

	auto carry = begin(counter);
	while(carry != end(counter))
	{
		std::invoke(std::forward<F>(f),
			std::inner_product(
				begin(primes), end(primes),
				begin(counter), prime_type{} + 1, // ugh
				std::multiplies{},
				pow<prime_type, power_type>
			)
		);
		carry = advance_vector(counter, powers);
	}
}

template <typename PrimeFactors, typename FactorPowers, typename Counter, typename OutItr>
constexpr void all_factors(const PrimeFactors& primes, const FactorPowers& powers, Counter&& counter, OutItr out)
{
	for_all_factors(primes, powers, std::forward<Counter>(counter),
		[&out](auto factor) { *out++ = factor; }
	);
}

#endif /* end of include guard */
