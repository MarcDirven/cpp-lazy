#pragma once
#include <array>


namespace detail
{
	template<class T, size_t N, class Iterator>
	std::array<T, N> makeArray(Iterator first)
	{
		std::array<T, N> arr = {};
		for (size_t i = 0; i < N; i++)
		{
			arr[i] = *first;
			++first;
		}
		return arr;
	}
}
