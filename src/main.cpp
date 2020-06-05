#include <iostream>

#include "RangeIterator.hpp"
#include "FilterIterator.hpp"
#include "SplitIterator.hpp"
#include "MapIterator.hpp"
#include "ZipIterator.h"


struct Test
{
	int a{}, b{};
};

template<typename... Args>
void args(int)
{
	
}


int main(int argc, char** argv)
{
	std::string string("Hello world I'm Marc");
	std::string delimiter(" ");
	auto splitted = lz::split(std::move(string), std::move(delimiter));

	std::array<Test, 3> x{ Test{2, 3}, Test{3, 4}, Test{5, 9} };
	auto func = [](const Test& t)
	{
		return t.a;
	};
	auto mapped = lz::map(x, func);

	std::array<int, 20> a{};
	std::array<float, 20> b{};
	auto z = lz::zip(a, b);
	auto beg = z.begin();
	*beg;
	
}
