#include <iostream>

#include "SplitIterator.hpp"

int main(int argc, char** argv)
{
	std::string myString = "Hello world e";
	for (auto& s : lz::split(myString, " "))
	{
		std::cout << s << std::endl;
	}
}
