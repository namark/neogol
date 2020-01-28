#include "utils.hpp"
#include <iostream>

using namespace std::chrono_literals;

frametime_logger::frametime_logger(size_t limit) : accum{}, count{}, limit{limit}
{}

void frametime_logger::log(clock::duration frametime)
{
	accum += frametime;
	++count;
	if(count == limit)
	{
		auto avg = accum/count;
		std::cout << avg.count() << '\n';
		count = 0;
		accum = 0s;
	}
}
