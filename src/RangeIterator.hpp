#pragma once
#include <iterator>
#include <vector>

#include "Tools.hpp"

namespace detail
{
	template<class IntType>
	class ConstRangeIterator
	{
		IntType _begin{};
		IntType _end{};
		IntType _step{};

	public:
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type = IntType;
		using difference_type = IntType;
		using pointer = IntType;
		using reference = IntType;
		
		ConstRangeIterator(IntType begin, IntType end, IntType step):
			_begin(begin),
			_end(end),
			_step(step)
		{
		}

		reference operator*() const
		{
			return _begin;
		}

		pointer operator->() const
		{
			return _begin;
		}

		bool operator!=(const ConstRangeIterator& other) const
		{
			if (_step < 0)
			{
				return _begin > other._end;
			}
			return _begin < other._end;
		}

		ConstRangeIterator& operator++()
		{
			_begin += _step;
			return *this;
		}

		ConstRangeIterator operator++(int) const
		{
			return ConstRangeIterator(_begin + _step, _end, _step);
		}
	};

	template<class IntType>
	class RangeIterator : public ConstRangeIterator<IntType>
	{
	public:
		RangeIterator(IntType begin, IntType end, IntType step):
			ConstRangeIterator<IntType>(begin, end, step)
		{
		}
	};
}

namespace lz
{
	template<class IntType>
	class RangeObject
	{
		IntType _begin{};
		IntType _end{};
		IntType _step{};

	public:
		using value_type = IntType;
		using size_type = size_t;
		using difference_type = ptrdiff_t;
		using pointer = IntType*;
		using const_pointer = const IntType*;
		using reference = IntType&;
		using const_reference = const IntType&;

		using iterator = detail::RangeIterator<IntType>;
		using const_iterator = detail::ConstRangeIterator<IntType>;

		RangeObject(IntType start, IntType end, IntType step):
			_begin(start),
			_end(end),
			_step(step)
		{
		}

		iterator begin()
		{
			return iterator(_begin, _end, _step);
		}
		
		iterator end()
		{
			return iterator(_begin, _end, _step);
		}

		const_iterator begin() const
		{
			return const_iterator(_begin, _end, _step);
		}

		const_iterator end() const
		{
			return const_iterator(_begin, _end, _step);
		}

		template<class T>
		T to()
		{
			return T(begin(), end());
		}
		
		std::vector<value_type> toVector() const
		{
			return to<std::vector<value_type>>();
		}

		template<size_t N>
		std::array<value_type, N> toArray() const
		{
			std::array<value_type, N> container;
			detail::fillContainer(begin(), container);
			return container;
		}
	};

	template<class IntType = int>
	RangeObject<IntType> range(const IntType start, const IntType end, const IntType step = 1)
	{
		static_assert(std::is_arithmetic<IntType>(), "type must be of type arithmetic");
		return RangeObject<IntType>(start, end, step);
	}

	template<class IntType = int>
	RangeObject<IntType> range(const IntType end)
	{
		return range(0, end, 1);
	}
}