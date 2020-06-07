#pragma once
#include <iterator>
#include <vector>

#include "../Tools.hpp"

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

		ConstRangeIterator operator++(int)
		{
			auto tmp = *this;
			++*this;
			return tmp;
		}

		ConstRangeIterator& operator--()
		{
			_begin -= _step;
			return *this;
		}

		ConstRangeIterator operator--(int)
		{
			auto tmp = *this;
			--*this;
			return tmp;
		}

		ConstRangeIterator& operator+=(const difference_type offset)
		{
			_begin += (offset * _step);
			return *this;
		}

		ConstRangeIterator operator+(const difference_type offset) const
		{
			auto* tmp = *this;
			return tmp += offset;
		}

		ConstRangeIterator& operator-=(const difference_type offset)
		{
			_begin -= (offset * _step);
			return *this;
		}

		ConstRangeIterator operator-(const difference_type other) const
		{
			auto tmp = *this;
			return tmp -= other;
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
		using iterator = detail::RangeIterator<IntType>;
		using const_iterator = detail::ConstRangeIterator<IntType>;

		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		using value_type = typename iterator::value_type;
		using size_type = size_t;
		using difference_type = typename iterator::difference_type;
		using pointer = typename iterator::pointer;
		using const_pointer = const pointer;
		using reference = typename iterator::reference;
		using const_reference = const reference;

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

		reverse_iterator rbegin()
		{
			return reverse_iterator(begin());
		}

		reverse_iterator rend()
		{
			return reverse_iterator(end());
		}
		
		const_reverse_iterator crbegin() const
		{
			return const_reverse_iterator(begin());
		}

		const_reverse_iterator crend() const
		{
			return const_reverse_iterator(end());
		}

		template<template<typename, typename...> class Container, typename... Args>
		Container<value_type, Args...> to() const
		{
			return Container<value_type, Args...>(begin(), end());
		}
		
		std::vector<value_type> toVector() const
		{
			return to<std::vector>();
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
		static_assert(std::is_arithmetic<IntType>::value, "type must be of type arithmetic");
		return RangeObject<IntType>(start, end, step);
	}

	template<class IntType = int>
	RangeObject<IntType> range(const IntType end)
	{
		return range(0, end, 1);
	}
}