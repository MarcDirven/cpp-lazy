#pragma once

#include "Tools.hpp"

#include <type_traits>
#include <algorithm>
#include <vector>

namespace detail
{
	template<class Container, class Function>
	class ConstFilterIterator
	{
		using ConstIterator = typename std::decay_t<Container>::const_iterator;
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = typename ConstIterator::value_type;
		using difference_type = typename ConstIterator::difference_type;
		using pointer = typename ConstIterator::pointer;
		using reference = typename ConstIterator::reference;

	private:
		ConstIterator _begin{};
		ConstIterator _end{};
		Function _function{nullptr};

	public:
		ConstFilterIterator(ConstIterator begin, ConstIterator end, Function function, const bool isEndIterator) :
			_begin(std::move(begin)),
			_end(std::move(end)),
			_function(function)
		{
			if (isEndIterator)
			{
				return;
			}
			_begin = std::find_if(_begin, _end, _function);
		}

		reference operator*() const
		{
			return *_begin;
		}

		pointer operator->() const
		{
			return _begin.operator->();
		}

		bool operator!=(const ConstFilterIterator& other) const
		{
			return _begin != other._end;
		}

		bool operator==(const ConstFilterIterator& other) const
		{
			return !(*this == other);
		}

		ConstFilterIterator& operator++()
		{
			if (_begin != _end)
			{
				_begin = std::find_if(_begin + 1, _end, _function);
			}
			return *this;
		}

		ConstFilterIterator operator++(int)
		{
			auto tmp = *this;
			++*this;
			return tmp;
		}

		ConstFilterIterator& operator+=(const difference_type offset)
		{
			for (difference_type i = 0; i < offset; ++i, ++*this)
			{
				if (_begin == _end)
				{
					break;
				}
			}
			return *this;
		}

		ConstFilterIterator operator+(const difference_type other) const
		{
			auto tmp = *this;
			tmp += other;
			return tmp;
		}
	};

	template<class Container, class Function>
	class FilterIterator : public ConstFilterIterator<Container, Function>
	{
		using iterator = typename std::decay_t<Container>::iterator;
		using Base = ConstFilterIterator<Container, Function>;
		
	public:
		using iterator_category = typename iterator::iterator_category;
		using value_type = typename iterator::value_type;
		using difference_type = typename iterator::difference_type;
		using pointer = typename iterator::pointer;
		using reference = typename iterator::reference;

		FilterIterator(iterator begin, iterator end, Function function, const bool isEndIterator):
			ConstFilterIterator<Container, Function>(begin, end, function, isEndIterator)
		{
		}

		reference operator*()
		{
			return const_cast<reference>(Base::operator*());
		}

		pointer operator->()
		{
			return const_cast<pointer>(Base::operator->());
		}

		bool operator!=(const FilterIterator& other) const
		{
			return Base::operator!=(other);
		}

		bool operator==(const FilterIterator& other) const
		{
			return Base::operator==(other);
		}
		
		FilterIterator& operator++()
		{
			Base::operator++();
			return *this;
		}

		FilterIterator operator++(int)
		{
			auto tmp = *this;
			Base::operator++();
			return tmp;
		}

		FilterIterator& operator+=(const difference_type offset)
		{
			Base::operator+=(offset);
			return *this;
		}

		FilterIterator operator+(const difference_type offset)
		{
			auto tmp = *this;
			return tmp += offset;
		}
	};
}

namespace lz
{
	template<class Container, class Function>
	class FilterObject
	{
		Container&& _container{};
		Function _function{nullptr};

		using ContainerAlias = std::decay_t<Container>;
	public:
		using value_type = typename ContainerAlias::value_type;
		using size_type = typename ContainerAlias::size_type;
		using difference_type = typename ContainerAlias::difference_type;
		using pointer = typename ContainerAlias::pointer;
		using const_pointer = typename ContainerAlias::const_pointer;
		using reference = typename ContainerAlias::reference;
		using const_reference = typename ContainerAlias::const_reference;

		using iterator = detail::FilterIterator<Container, Function>;
		using const_iterator = detail::ConstFilterIterator<Container, Function>;

		FilterObject(Container&& container, Function function):
			_container(std::forward<Container>(container)),
			_function(function)
		{
		}

		iterator begin()
		{
			return iterator(_container.begin(), _container.end(), _function, false);
		}

		iterator end()
		{
			return iterator(_container.begin(), _container.end(), _function, true);
		}
		
		const_iterator begin() const
		{
			return const_iterator(_container.begin(), _container.end(), _function, false);
		}

		const_iterator end() const
		{
			return const_iterator(_container.begin(), _container.end(), _function, true);
		}

		template<template<typename, typename...> class ContainerType, typename... Args>
		ContainerType<value_type, Args...> to() const
		{
			return ContainerType<value_type, Args...>(begin(), end());
		}

		std::vector<value_type> toVector() const
		{
			return to<std::vector>();
		}

		template<size_t N>
		std::array<value_type, N> toArray() const
		{
			std::array<value_type, N> container{};
			detail::fillContainer(begin(), container);
			return container;
		}
	};
	
	template<class Container, class Function>
	FilterObject<Container, Function> filter(Container&& container, Function predicate)
	{
		using FunctionReturnType = typename std::result_of<Function(typename std::decay_t<Container>::value_type)>::type;
		static_assert(detail::IsCallable<Function>::value, "template parameter Function is not a function");
		static_assert(std::is_same<bool, FunctionReturnType>::value, "function must return a bool");
		
		return FilterObject<Container, Function>(std::forward<Container>(container), predicate);
	}
}