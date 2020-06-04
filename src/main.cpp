#include <iostream>

#include "RangeIterator.hpp"
#include "SplitIterator.hpp"
#include "list"


int main(int argc, char** argv)
{
	auto container = lz::split("hello world", " ").to<std::vector>();
}
