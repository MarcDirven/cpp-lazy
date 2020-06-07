#include <iostream>

#include "include/RangeIterator.hpp"
#include "include/FilterIterator.hpp"
#include "include/SplitIterator.hpp"
#include "include/MapIterator.hpp"
#include "include/ZipIterator.h"


struct Test
{
	int a{}, b{};
	using value_type = int;
};

template<typename... Args>
std::tuple<typename Args::value_type...> args(Args... test)
{
	return { (test.a)... };
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

	std::array<int, 2> a{};
	std::array<float, 1> b{};
	std::array<int, 3> c{};

	auto zipper = lz::zip(a, b, c);

	for (auto tuple : zipper)
	{
		std::get<0>(tuple) = 20;
	}
}
