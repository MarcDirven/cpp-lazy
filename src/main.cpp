#include "RangeIterator.hpp"
#include "WhereIterator.hpp"
#include "SplitIterator.hpp"


bool isEven(int num)
{
	return num % 2 == 0;
}

bool isHello(const std::string& s)
{
	return s == "Hello";
}


int main(int argc, char** argv)
{
	std::string s = "Hello world wassup";
	auto x = lz::split(s, " ").toVector();
}
