#pragma once


namespace detail
{
	template<class Container, class Iterator>
	void fillContainer(Iterator first, Container& container)
	{
		for (size_t i = 0; i < container.size(); i++)
		{
			container[i] = *first;
			++first;
		}
	}
}
