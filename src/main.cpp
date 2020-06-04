#include <iostream>

#include "RangeIterator.hpp"
#include "FilterIterator.hpp"
#include "SplitIterator.hpp"
#include "MapIterator.hpp"


struct Test
{
	int a{}, b{};
};



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
	std::reverse(mapped.begin(), mapped.end());
}
